#include "externManager.h"
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")
//#include "C:\Users\dasha\AppData\Local\Programs\Python\Python310\include\Python.h"

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