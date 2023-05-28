#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GestureEditor.h"
#include "SignLanguageWriter.h"
#include "RobotConnectionManager.h"
#include "../utils/MediapipeInterface.h"
#include "../GestureLibrary.h"

class GestureLibrary;

namespace Ui {
class MainWindow;
}

/**
 * The main window for the entire application
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

/**
 * Draws a new frame on the window.
 * To be used for realtime recognition.
 * @param image The image to draw. Looks best with 16:9 aspect ratio.
 */
    void paintRealtimeFrame(QImage& image);

private slots:
    // GUI button click handlers

    void on_actionNew_Gesture_triggered();
    void on_actionSign_Language_triggered();
    void on_actionRobot_triggered();
    void on_cameraToggle_clicked();
    void on_actionSave_all_Gestures_triggered();
    void on_actionLoad_Gesture_Library_triggered();
    void on_videoPickButton_clicked();
    void on_recognizeButton_clicked();

/**
 * Wrapper method for GestureLibrary::realtimeRecognition().
 * If a gesture is detected, the GUI and the potential opened application get updated.
 * @param landmarks Landmark data: on-screen coordinates of landmarks for the current frame.
 */

    void tryRealtimeRecognition(const std::vector<double> &landmarks);

private:
    Ui::MainWindow *ui;
    SignLanguageWriter* signLanguageWriter;
    RobotConnectionManager* robotConnectionManager;
    MediapipeInterface mediaPipeInterface;
    GestureLibrary* library;
    QString videoInputPath;

/**
 * Synchronizes the GUI with the recognizable gestures as held by the gesture library.
 */

    void refreshGesturesView();

/**
 * Updates the GUI and the potential opened application.
 * @param gestureID The ID of the gesture that was recognized
 */
    void handleGestureRecognized(std::string &gestureID);
};

#endif // MAINWINDOW_H
