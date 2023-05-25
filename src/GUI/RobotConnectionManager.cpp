#include <QMessageBox>
#include "RobotConnectionManager.h"
#include "ui_RobotConnectionManager.h"

RobotConnectionManager::RobotConnectionManager(QWidget *parent) : ApplicationExampleWindow(parent),
    ui(new Ui::RobotConnectionManager)
{
    ui->setupUi(this);
    controller = new RobotController();
    QObject::connect(controller, &RobotController::connected,
                     this, &RobotConnectionManager::onConnection);
    QObject::connect(controller, &RobotController::connectionTestSuccess,
                     this, &RobotConnectionManager::onConnectionTestSuccess);
}

RobotConnectionManager::~RobotConnectionManager()
{
    delete ui;
}

void RobotConnectionManager::on_connectButton_clicked()
{
    controller->attemptConnection();
}


void RobotConnectionManager::on_IPlineEdit_returnPressed()
{
    controller->setIp(ui->IPlineEdit->text());
}


void RobotConnectionManager::on_testConnectionButton_clicked()
{
    controller->testConnection();
}

void RobotConnectionManager::onConnection() {
    ui->statusLabel->setText("Connected");
}


void RobotConnectionManager::on_turnOffButton_clicked()
{
    controller->abort();
}

void RobotConnectionManager::onConnectionTestSuccess() {
    QMessageBox msgBox;
    msgBox.setText("The robot responded: the connection is active.");
    msgBox.exec();
}

void RobotConnectionManager::handleGestureRecognized(string &gestureID) {
    int index = std::distance(gestureNames.begin(), std::find(std::begin(gestureNames), std::end(gestureNames), gestureID));
    switch (index) {
        case 0:{
            controller->forward();
            break;
        }
        case 1:{
            controller->backward();
            break;
        }
        case 2:{
            controller->left();
            break;
        }
        case 3:{
            controller->right();
            break;
        }
        case 4:{
            controller->stop();
            break;
        }
    }
}

