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
    void dataAvailable(std::vector<double> data);
private slots:
    void acceptConnection();
    void onDataReady();

private:
    QTcpSocket* imageConnection;
    QTcpSocket* dataConnection;
    QTcpServer imageServer;
    QTcpServer dataServer;
    bool isOpened;
public:
    static std::vector<std::vector<double>> getLandmarksFromVideo(const char* absoluteVideoPath);
    static std::vector<int> preprocessData(const std::vector<std::vector<double>>& data); // Data of one video
    static std::vector<std::vector<int>> preprocessData(const std::vector<std::vector<std::vector<double>>>& data); // Data of multiple videos
};


#endif //HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H
