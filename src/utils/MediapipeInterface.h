#ifndef HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H
#define HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>

class MediapipeInterface : public QObject {
Q_OBJECT

public:
    MediapipeInterface();
    ~MediapipeInterface() override;
    bool open();
    bool close();
    bool isOpen() const;
    std::vector<std::vector<double>> getLandmarksFromVideo(const char* absoluteVideoPath);
    static std::vector<int> preprocessData(const std::vector<std::vector<double>>& data); // Data of one video
    static std::vector<std::vector<int>> preprocessData(const std::vector<std::vector<std::vector<double>>>& data); // Data of multiple videos
    static double getXRange(const std::vector<std::vector<double>>& dataToAnalyse);
    static double getYRange(const std::vector<std::vector<double>>& dataToAnalyse);
    static std::map<std::string, bool> getFiltersFromData(const std::vector<std::vector<std::vector<double>>>& dataToAnalyse);
    static std::map<std::string, bool> getFiltersFromData(const std::vector<std::vector<double>>& dataToAnalyse);

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
    QProcess* pythonClients;
    bool isOpened;

};


#endif //HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H
