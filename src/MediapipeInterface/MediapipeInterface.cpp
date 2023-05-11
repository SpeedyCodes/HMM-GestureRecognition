#include "MediapipeInterface.h"
#include <QDebug>
#include <QStringListModel>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QLayout>

MediapipeInterface::MediapipeInterface() {
    isOpened = false;
}

void MediapipeInterface::acceptConnection()
{
    connection = server.nextPendingConnection();
    if (!connection) {
        qDebug() << "Error: got invalid pending connection!";
        return;
    }
    connect(connection, &QIODevice::readyRead,
            this, &MediapipeInterface::onDataReady);

    qDebug() << "Accepted connection";
    server.close();
}

void MediapipeInterface::onDataReady()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray data = sender->readAll();
    QImage image;
    image.loadFromData(data);
    emit(imageAvailable(image));
}

bool MediapipeInterface::open() {
    if(isOpened) return false;
    connect(&server, &QTcpServer::newConnection, this, &MediapipeInterface::acceptConnection);
    server.listen(QHostAddress::Any, 5000);
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
    connection->write(QByteArray::fromStdString("abort"));
    connection->abort();
    return true;
}