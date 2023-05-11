#ifndef HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H
#define HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H

#include <QTcpServer>
#include <QTcpSocket>

class MediapipeInterface : public QObject {
Q_OBJECT
public:
    MediapipeInterface();
    bool open();
    bool close();

signals:
    void imageAvailable(QImage& image);
private slots:
    void acceptConnection();
    void onDataReady();

private:
    QTcpSocket* connection;
    QTcpServer server;
    bool isOpened;
};


#endif //HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H
