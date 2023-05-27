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
    Py_Initialize();
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
        double* x = (double*)data.data();
        double* y = x + 1;
        double* lefth = y + 1;
        emit(dataAvailable({*x, *y, *lefth}));
    }
}

bool MediapipeInterface::open() {
    if(isOpened) return false;
    connect(&imageServer, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    imageServer.listen(QHostAddress::Any, 5000);
    connect(&dataServer, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    dataServer.listen(QHostAddress::Any, 5001);
    pythonClients = new QProcess;
    pythonClients->start("python", {"src/utils/run_tcp.py"}); // TODO what to do with systems on which the command is python3?
    isOpened = true;
    return isOpened;
}

bool MediapipeInterface::close() {
    if(!isOpened) return false;
    disconnect(&imageServer, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    disconnect(&dataServer, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    disconnect(imageConnection, &QIODevice::readyRead,
            this, &MediapipeInterface::onDataReady);
    disconnect(dataConnection, &QIODevice::readyRead,
            this, &MediapipeInterface::onDataReady);
    imageConnection->write(QByteArray::fromStdString("abort"));
    imageConnection->abort();
    dataConnection->write(QByteArray::fromStdString("abort"));
    dataConnection->abort();
    imageConnection->deleteLater();
    dataConnection->deleteLater();
    pythonClients->terminate(); //TODO replace with something a little more memory-safe https://stackoverflow.com/questions/45927337/recieve-data-only-if-available-in-python-sockets
    delete pythonClients;
    imageConnection = nullptr;
    dataConnection = nullptr;
    isOpened = false;
    return isOpened;
}

MediapipeInterface::~MediapipeInterface() {
    close();
    Py_Finalize();
}
std::vector<std::vector<double>> MediapipeInterface::getLandmarksFromVideo(const char* videoPath){
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

    // Import the "get_landmarks" module
    PyObject* module_name = PyUnicode_FromString("get_landmarks");
    PyObject* module = PyImport_Import(module_name);
    Py_DECREF(module_name);

    if (module == NULL) {
        PyErr_Print();
    }

    // Get a reference to the "get_landmarks_from_video" function
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
    // Call the function
    PyObject* pResult = PyObject_CallObject(func, args);

    // Extract results
    std::vector<std::vector<double>> result;
    if(pResult == NULL) std::cerr << "Got nothing from get_landmarks.py. Did you give the absolute path?"  << std::endl;
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

    return result;
}
std::vector<int> MediapipeInterface::preprocessData(const std::vector<std::vector<double>>& data){
    const int magicNumber = 20; // parameter of the vector quantization TODO: set it in config
    std::vector<int> to_return;
    if(data.size() < 2){
        std::cerr << "The given vector of data is empty or has only one element" << std::endl;
        return to_return;
    }
    std::vector<double> previousFrameData;
    bool firstFrame = true;
    for(const std::vector<double>& frameData: data){
        if(frameData.empty()) {
            std::cerr << "Frame data is empty" <<std::endl;
            continue;
        }
        if(firstFrame){
            if(frameData[0] == -1 and frameData[1] == -1) continue; // For realime recognition
            previousFrameData = frameData;
            firstFrame = false;
        }else{
            double angle; // We use angle as the feature
            if(frameData[0] != -1 or frameData[1] != -1){
                double x = frameData[0] - previousFrameData[0];
                double y = frameData[1] - previousFrameData[1];
                if (x == 0)
                {
                    if (y > 0) angle = M_PI / 2;
                    angle = -M_PI / 2;
                }
                else if (x < 0)
                {
                    angle = atan(y / x) + M_PI;
                }
                else{
                    angle = atan(y / x);
                }
                angle = angle * 180.0 / M_PI; // Set to degrees
                if(angle < 0) angle += 360;
                angle /= magicNumber;
                previousFrameData = frameData;
            }else{ // Realtime recognition, the hand is not in the frame
                angle = -1;
                previousFrameData = {0,0};
            }
            to_return.push_back(static_cast<int>(std::round(angle)));
        }
    }
    return to_return;
}
std::vector<std::vector<int>> MediapipeInterface::preprocessData(const std::vector<std::vector<std::vector<double>>>& data){
    std::vector<std::vector<int>> to_return;
    for(const std::vector<std::vector<double>>& singleVector: data){
        std::vector<int> to_add = preprocessData(singleVector);
        if(!to_add.empty()) to_return.push_back(to_add);
    }
    return to_return;
}

bool MediapipeInterface::isOpen() const {
    return isOpened;
}

double MediapipeInterface::getXRange(const std::vector<std::vector<double>>& dataToAnalyse){
    if(dataToAnalyse.empty()) return 0;
    double x_min = std::numeric_limits<double>::infinity();
    double x_max = -std::numeric_limits<double>::infinity();
    for(auto vect:dataToAnalyse){
        if(vect.empty()) continue;
        if(vect[0] > x_max) x_max = vect[0];
        if(vect[0] < x_min) x_min = vect[0];
    }
    if(x_min == std::numeric_limits<double>::infinity() || x_max == -std::numeric_limits<double>::infinity()) return 0;
    return x_max-x_min;
}
double MediapipeInterface::getYRange(const std::vector<std::vector<double>>& dataToAnalyse){
    if(dataToAnalyse.empty()) return 0;
    double x_min = std::numeric_limits<double>::infinity();
    double x_max = -std::numeric_limits<double>::infinity();
    for(auto vect:dataToAnalyse){
        if(vect.empty()) continue;
        if(vect[1] > x_max) x_max = vect[1];
        if(vect[1] < x_min) x_min = vect[1];
    }
    if(x_min == std::numeric_limits<double>::infinity() || x_max == -std::numeric_limits<double>::infinity()) return 0;
    return x_max-x_min;
}

std::map<std::string, bool> MediapipeInterface::getFiltersFromData(const std::vector<std::vector<double>>& dataToAnalyse){
    // Calculate global features
    std::map<std::string, bool> to_return;
    double x_range = MediapipeInterface::getXRange(dataToAnalyse);
    double y_range = MediapipeInterface::getYRange(dataToAnalyse);
    // Global feature 1: mini-gesture (x- and y-range are <25%)
    if(x_range <= 0.25 && y_range <= 0.25) to_return.insert(std::make_pair("mini", true));
    else to_return.insert(std::make_pair("mini", false));
    // Global feature 2: almost full (>85%) x-range
    if(x_range >= 0.85) to_return.insert(std::make_pair("fullx", true));
    else to_return.insert(std::make_pair("fullx", false));
    // Global feature 3: almost full (>85%) y-range
    if(y_range >= 0.85) to_return.insert(std::make_pair("fully", true));
    else to_return.insert(std::make_pair("fully", false));
    // Global feature 4: two hands
    unsigned int lefth = 0;
    for(auto vec: dataToAnalyse){
        if(vec.size() < 3) continue;
        if(vec[2] == -1) lefth++;
    }
    if(lefth >= dataToAnalyse.size()*0.85) to_return.insert(std::make_pair("left", true));
    else to_return.insert(std::make_pair("left", false));

    return to_return;
}
