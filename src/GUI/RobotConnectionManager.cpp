#include "RobotConnectionManager.h"
#include "ui_RobotConnectionManager.h"

RobotConnectionManager::RobotConnectionManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RobotConnectionManager)
{
    ui->setupUi(this);
}

RobotConnectionManager::~RobotConnectionManager()
{
    delete ui;
}
