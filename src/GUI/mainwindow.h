#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GestureEditor.h"
#include "SignLanguageWriter.h"
#include "RobotConnectionManager.h"
#include "../MediapipeInterface/MediapipeInterface.h"


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

private:
    Ui::MainWindow *ui;
    SignLanguageWriter* signLanguageWriter;
    RobotConnectionManager* robotConnectionManager;
    MediapipeInterface mediaPipeInterface;

};

#endif // MAINWINDOW_H
