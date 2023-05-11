//
// Created by dasha on 10.05.2023.
//

#include "externManager.h"
#include <Python.h>

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
    PyObject* module_name = PyUnicode_FromString("collect_data");
    PyObject* module = PyImport_Import(module_name);
    Py_DECREF(module_name);

    if (module == NULL) {
        PyErr_Print();
    }

    // Get a reference to the "start_collection" function
    PyObject* func_name = PyUnicode_FromString("get_landmarks");
    PyObject* func = PyObject_GetAttr(module, func_name);
    Py_DECREF(func_name);

    if (!PyCallable_Check(func)) {
        Py_DECREF(func);
        Py_DECREF(module);
        PyErr_Print();
    }

    // Set arguments
    PyObject* py_relativeFolderPath = PyUnicode_FromString(videoPath);
    PyObject *args = PyTuple_Pack(1, py_relativeFolderPath);
    // Call the function with no arguments
    PyObject_CallObject(func, args);

    // Clean up
    Py_DECREF(func);
    Py_DECREF(module);
    Py_Finalize();
}