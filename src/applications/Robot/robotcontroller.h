#ifndef HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H
#define HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H

/**
 * A class to abstract away communications with the DIY remote-controlled car Jesse revived as a physical
 * demonstration of what gesture recognition may be used for
 */

#include <QTcpSocket>
#include "QObject"
#include <QHostAddress>

using namespace std;

class RobotController : public QObject
{
    Q_OBJECT
signals:
    void connected();
    void connectionTestSuccess();
public:
    RobotController();
    void attemptConnection();
    void forward();
    void backward();
    void left();
    void right();
    void stop();
    void abort();
    void testConnection();

    void setIp(const QString &ip);
private slots:
    void receiveData();

private:
    QTcpSocket socket;
    QHostAddress ip;
};

#endif // HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H
