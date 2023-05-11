#ifndef HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H
#define HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H

#include <QTcpServer>
#include <QTcpSocket>

class MediapipeInterface : public QObject {
Q_OBJECT

public:
    ~MediapipeInterface() override;

public:
    MediapipeInterface();
    bool open();
    bool close();

signals:
    void imageAvailable(QImage& image);
    void dataAvailable(int data);
private slots:
    void acceptConnection();
    void onDataReady();

private:
    QTcpSocket* imageConnection;
    QTcpSocket* dataConnection;
    QTcpServer imageServer;
    QTcpServer dataServer;
    bool isOpened;
};


#endif //HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H
