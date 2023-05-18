#ifndef ROBOTCONNECTIONMANAGER_H
#define ROBOTCONNECTIONMANAGER_H

#include <QMainWindow>
#include "../applications/Robot/robotcontroller.h"

namespace Ui {
class RobotConnectionManager;
}

class RobotConnectionManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit RobotConnectionManager(QWidget *parent = nullptr);
    ~RobotConnectionManager();

private slots:
    void onConnection();

    void onConnectionTestSuccess();

    void on_connectButton_clicked();

    void on_IPlineEdit_returnPressed();

    void on_testConnectionButton_clicked();

    void on_turnOffButton_clicked();

private:
    Ui::RobotConnectionManager *ui;
    RobotController* controller;
};

#endif // ROBOTCONNECTIONMANAGER_H
