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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void paintRealtimeFrame(QImage& image);

private slots:
    void on_actionNew_Gesture_triggered();

    void on_actionSign_Language_triggered();

    void on_actionRobot_triggered();

    void on_cameraToggle_clicked();

    void on_actionSave_all_Gestures_triggered();

    void on_actionLoad_Gesture_Library_triggered();

    void on_videoPickButton_clicked();

    void on_recognizeButton_clicked();

    void tryRealtimeRecognition(const std::vector<double> &landmarks);

    void setMultiple(bool);

private:
    Ui::MainWindow *ui;
    SignLanguageWriter* signLanguageWriter;
    RobotConnectionManager* robotConnectionManager;
    MediapipeInterface mediaPipeInterface;
    GestureLibrary* library;
    QString videoInputPath;
    
    void refreshGesturesView();
    void handleGestureRecognized(std::string &gestureID);
};

#endif // MAINWINDOW_H
