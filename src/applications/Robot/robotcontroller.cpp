#include "robotcontroller.h"

RobotController::RobotController() : socket(this)
{
    QObject::connect(&socket, &QTcpSocket::connected,
                     this, &RobotController::connected);
    QObject::connect(&socket, &QIODevice::readyRead,
            this, &RobotController::receiveData);
}

void RobotController::attemptConnection() {
    if (ip.isNull()) return;
    socket.connectToHost(ip, 3000);
}

void RobotController::forward()
{
    socket.write(QByteArray("forward"));
}

void RobotController::backward()
{
    socket.write(QByteArray("backward"));
}

void RobotController::left()
{
    socket.write(QByteArray("left"));
}

void RobotController::right()
{
    socket.write(QByteArray("right"));
}

void RobotController::stop()
{
    socket.write(QByteArray("stop"));
}

void RobotController::abort()
{
    socket.write(QByteArray("abort"));
}

void RobotController::setIp(const QString &ip) {
    RobotController::ip = QHostAddress(ip);
}

void RobotController::testConnection() {
    socket.write(QByteArray("ping"));
}

void RobotController::receiveData() {
    QByteArray data = socket.readAll();
    if(data.toStdString() == "pong"){
        emit connectionTestSuccess();
    }
}
