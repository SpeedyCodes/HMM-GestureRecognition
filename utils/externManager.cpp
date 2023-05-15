#include "externManager.h"
#include <cmath>
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")

std::vector<std::vector<double>> externManager::getLandmarksFromVideo(const char* videoPath){
    Py_Initialize();

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        std::cerr << "Error getting current working directory" << std::endl;
    }
    // Get the right file
    PyObject* sys_module = PyImport_ImportModule("sys");
    PyObject* path = PyObject_GetAttrString(sys_module, "path");
    PyList_Append(path, PyUnicode_FromString(cwd));
    PyList_Append(path, PyUnicode_FromString("utils"));
    Py_DECREF(sys_module);

    // Import the "collect_data" module
    PyObject* module_name = PyUnicode_FromString("get_landmarks");
    PyObject* module = PyImport_Import(module_name);
    Py_DECREF(module_name);

    if (module == NULL) {
        PyErr_Print();
    }

    // Get a reference to the "start_collection" function
    PyObject* func_name = PyUnicode_FromString("get_landmarks_from_video");
    PyObject* func = PyObject_GetAttr(module, func_name);
    Py_DECREF(func_name);

    if (!PyCallable_Check(func)) {
        Py_DECREF(func);
        Py_DECREF(module);
        PyErr_Print();
    }

    // Set arguments
    PyObject* py_absoluteVideoPath = PyUnicode_FromString(videoPath);
    PyObject *args = PyTuple_Pack(1, py_absoluteVideoPath);
    // Call the function with no arguments
    PyObject* pResult = PyObject_CallObject(func, args);

    // Extract result
    std::vector<std::vector<double>> result;
    if(pResult == NULL) std::cerr << "Got nothing from get_landmarks_from_video.py. Did you give the absolute path?"  << std::endl;
    if (PyList_Check(pResult)) {
        Py_ssize_t outerSize = PyList_Size(pResult);
        for (Py_ssize_t i = 0; i < outerSize; ++i) {
            PyObject* pInnerList = PyList_GetItem(pResult, i);
            if (PyList_Check(pInnerList)) {
                std::vector<double> innerVec;
                Py_ssize_t innerSize = PyList_Size(pInnerList);
                for (Py_ssize_t j = 0; j < innerSize; ++j) {
                    PyObject* pValue = PyList_GetItem(pInnerList, j);
                    double value = PyFloat_AsDouble(pValue);
                    innerVec.push_back(value);
                }
                result.push_back(innerVec);
            }
        }
    }

    // Clean up
    Py_DECREF(func);
    Py_DECREF(module);
    Py_Finalize();

    return result;
}
std::vector<int> externManager::preprocessData(const std::vector<std::vector<double>>& data){
    const int magicNumber = 20; // parameter of the vector quantization
    std::vector<int> to_return;
    if(data.size() < 2){
        std::cerr << "The given vector of data is empty or has only one element" << std::endl;
        return to_return;
    }
    std::vector<double> previousFrameData;
    bool firstFrame = true;
    for(const std::vector<double>& frameData: data){
        if(firstFrame){
            previousFrameData = frameData;
            firstFrame = false;
        }else{
            double angle; // We use angle as the feature
            if(frameData[0] - previousFrameData[0] != 0){
                // Use arctan
                angle = atan((frameData[1] - previousFrameData[1])/(frameData[0] - previousFrameData[0]));
            }else if(frameData[1] - previousFrameData[1] > 0){
                angle = M_PI/2;
            }else if(frameData[1] - previousFrameData[1] < 0){
                angle = -M_PI/2;
            }else{ // 0/0 => 0
                angle = 0;
            }
            // TODO: special number for absent observations + remove end trash?
            angle = angle * 180.0 / M_PI; // Set to degrees
            if(angle < 0) angle += 360;
            angle /= magicNumber;
            to_return.push_back(static_cast<int>(std::round(angle)));
            previousFrameData = frameData;
        }
    }
    return to_return;
}
std::vector<std::vector<int>> externManager::preprocessData(const std::vector<std::vector<std::vector<double>>>& data){
    std::vector<std::vector<int>> to_return;
    for(const std::vector<std::vector<double>>& singleVector: data){
        std::vector<int> to_add = preprocessData(singleVector);
        to_return.push_back(to_add);
    }
    return to_return;
}