#include "MediapipeInterface.h"
#include <QDebug>
#include <QStringListModel>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QLayout>
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")
#include <iostream>

MediapipeInterface::MediapipeInterface() {
    isOpened = false;
    imageConnection = nullptr;
    dataConnection = nullptr;
}

void MediapipeInterface::acceptConnection()
{
    QTcpServer* sender = static_cast<QTcpServer*>(QObject::sender());
    QTcpSocket* connection = sender->nextPendingConnection();
    if (!connection) {
        qDebug() << "Error: got invalid pending connection!";
        return;
    }
    connect(connection, &QIODevice::readyRead,
            this, &MediapipeInterface::onDataReady);

    qDebug() << "Accepted connection";
    sender->close();
    if(imageConnection == nullptr) imageConnection = connection;
    else dataConnection = connection;
}

void MediapipeInterface::onDataReady()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray data = sender->readAll();
    if(sender == imageConnection){
        QImage image;
        image.loadFromData(data);
        emit(imageAvailable(image));
    }else{
        bool ok;
        //TODO read data as whatever we need: int/double, array of int/double?
        std::vector<double> result;
        emit(dataAvailable(result));
    }
}

bool MediapipeInterface::open() {
    if(isOpened) return false;
    connect(&imageServer, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    imageServer.listen(QHostAddress::Any, 5000);
    connect(&dataServer, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    dataServer.listen(QHostAddress::Any, 5001);

    Py_Initialize();

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        std::cerr << "Error getting current working directory" << std::endl;
    }
    // Get the right file
    PyObject* sys_module = PyImport_ImportModule("sys");
    PyObject* path = PyObject_GetAttrString(sys_module, "path");
    PyList_Append(path, PyUnicode_FromString(cwd));
    PyList_Append(path, PyUnicode_FromString("src/utils"));
    Py_DECREF(sys_module);

    // Import the "run_tcp" module
    PyObject* module_name = PyUnicode_FromString("run_tcp");
    PyObject* module = PyImport_Import(module_name);
    Py_DECREF(module_name);

    if (module == NULL) {
        PyErr_Print();
    }

    // Get a reference to the "run_tcp_sewer" function
    PyObject* func_name = PyUnicode_FromString("run_tcp_sewer");
    PyObject* func = PyObject_GetAttr(module, func_name);
    Py_DECREF(func_name);

    if (!PyCallable_Check(func)) {
        Py_DECREF(func);
        Py_DECREF(module);
        PyErr_Print();
    }

    // Call the function with no arguments
    PyObject_CallObject(func, NULL);

    // Clean up
    Py_DECREF(func);
    Py_DECREF(module);
    Py_Finalize();

    isOpened = true;
    return true;
}

bool MediapipeInterface::close() {
    if(!isOpened) return false;
    imageConnection->write(QByteArray::fromStdString("abort"));
    imageConnection->abort();
    dataConnection->abort();
    return true;
}

MediapipeInterface::~MediapipeInterface() {
    close();
}
