#include "robotcontroller.h"

RobotController::RobotController(const QString& ipAddress)
{
    socket.connectToHost(QHostAddress(ipAddress), 3000);
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

