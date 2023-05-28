#ifndef HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H
#define HMM_GESTURERECOGNITION_MEDIAPIPEINTERFACE_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>

typedef int Observable;

/**
 * Wrapper class to interface with Python and Mediapipe for video processing
 */
class MediapipeInterface : public QObject {
Q_OBJECT

public:
/**
 * Sets up the connection to Python.
 * Warning: due to the nature of the connection, this can only happen once in the lifetime of the program.
 */

    MediapipeInterface();

/**
 * Disconnects from Python.
 */

    ~MediapipeInterface() override;

/**
 * Opens the Python server for realtime camera feed processing.
 * Frames and the data extracted from them become available through the imageAvailable and dataAvailable signals.
 * @return True if the interface was closed at first but was opened correctly.
 */

    bool open();

/**
 * Stops the Python server for realtime camera feed processing.
 * @return True if the interface was opened at first but was closed correctly.
 */

    bool close();

/**
 * @return True if the connection with the Python server is open.
 */

    bool isOpen() const;

/**
 * Gets landmark data from a video at a given path.
 * @param absoluteVideoPath The absolute path to the video
 * @return The landmarks
 */

    std::vector<std::vector<double>> getLandmarksFromVideo(const char* absoluteVideoPath) const;

/**
 * Turns landmark data into HMM observables: the angle between the vector connecting two consecutive landmarks and the x-axis.
 * @param data Landmark data
 * @return The observables
 */

    static std::vector<Observable> preprocessData(const std::vector<std::vector<double>>& data);

/**
 * Wrapper for multiple series of landmarks.
 * To be used with data from multiple videos, for example.
 * @param data Landmark data
 * @return The observables
 */

    static std::vector<std::vector<Observable>> preprocessData(const std::vector<std::vector<std::vector<double>>>& data);

signals:

/**
 * Indicates a new frame for realtime recognition is available
 * @param image The image
 */

    void imageAvailable(QImage& image);

/**
 * Indicates landmark data from a new frame for realtime recognition is available
 * @param data The landmark data
 */
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
