#include <QMessageBox>
#include "RobotConnectionManager.h"
#include "ui_RobotConnectionManager.h"

RobotConnectionManager::RobotConnectionManager(QWidget *parent) :
    QMainWindow(parent),
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

