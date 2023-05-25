#ifndef APPLICATIONEXAMPLEWINDOW_H
#define APPLICATIONEXAMPLEWINDOW_H

#include <QMainWindow>
#include "../applications/Robot/robotcontroller.h"

namespace Ui {
class ApplicationExampleWindow;
}

class ApplicationExampleWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void closed();
public:
    explicit ApplicationExampleWindow(QWidget *parent = nullptr) : QMainWindow(parent){};
    ~ApplicationExampleWindow(){};
    virtual void handleGestureRecognized(string &gestureID) = 0;
    void closeEvent(QCloseEvent *event) override;
};


#endif // APPLICATIONEXAMPLEWINDOW_H
