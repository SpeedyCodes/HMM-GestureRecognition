#ifndef ROBOTCONNECTIONMANAGER_H
#define ROBOTCONNECTIONMANAGER_H

#include <QMainWindow>

namespace Ui {
class RobotConnectionManager;
}

class RobotConnectionManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit RobotConnectionManager(QWidget *parent = nullptr);
    ~RobotConnectionManager();

private:
    Ui::RobotConnectionManager *ui;
};

#endif // ROBOTCONNECTIONMANAGER_H
