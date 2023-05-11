#include "MediapipeInterface.h"
#include <QDebug>
#include <QStringListModel>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QLayout>

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
        int result = 0;
        emit(dataAvailable(result));
    }
}

bool MediapipeInterface::open() {
    if(isOpened) return false;
    connect(&imageServer, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    imageServer.listen(QHostAddress::Any, 5000);
    connect(&dataServer, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    dataServer.listen(QHostAddress::Any, 5001);
    // TODO Running the python server automagically, platform-independently (Python.h?)
//    std::string filename = "../src/MediapipeInterface/main.py";
//    std::string command = "python ";
//    command += filename;
//    system(command.c_str());
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
