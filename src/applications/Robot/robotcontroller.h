#ifndef HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H
#define HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H

/**
 * A class to abstract away communications with the remote-controlled vehicle Jesse revived as a physical
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
/**
 * Indicates a connection has been established.
 */

    void connected();

/**
 * Indicates the connection is still alive.
 */

    void connectionTestSuccess();
public:
    RobotController();

/**
 * Starts attempting to connect to the robot at the IP address specified with setIp().
 * Does nothing if no address has been specified.
 */

    void attemptConnection();

/**
 * Sends a test message to the robot to verify that the connection is alive.
 */

    void testConnection();

/**
 * Command to the robot to go forward
 */

    void forward();

/**
 * Command to the robot to go backward
 */

    void backward();

/**
* Command to the robot to turn left
*/

    void left();

/**
* Command to the robot to turn right
*/

    void right();

/**
* Command to the robot to stop
*/

    void stop();

/**
 * Command to the robot to abort the connection
 */

    void abort();

/**
 * Sets the IP address of the robot.
 * @param ip The IPv4 address.
 */

    void setIp(const QString &ip);
private slots:
    void receiveData();
private:
    QTcpSocket socket;
    QHostAddress ip;
};

#endif // HMM_GESTURERECOGNITION_ROBOTCONTROLLER_H
