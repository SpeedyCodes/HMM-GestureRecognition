#ifndef HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H
#define HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H

/**
 * A class to abstract away communications with the DIY remote-controlled car Jesse revived as a physical
 * demonstration of what gesture recognition may be used for
 */

#include <QTcpSocket>

using namespace std;

class RobotController
{
public:
    RobotController(const QString& ipAddress);
    void forward();
    void backward();
    void left();
    void right();
    void stop();
    void abort();

private:
    QTcpSocket socket;
};

#endif // HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H
