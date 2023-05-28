#ifndef APPLICATIONEXAMPLEWINDOW_H
#define APPLICATIONEXAMPLEWINDOW_H

#include <QMainWindow>
#include "../applications/Robot/robotcontroller.h"

namespace Ui {
class ApplicationExampleWindow;
}

/**
 * Abstract class for windows for examples of applications.
 */
class ApplicationExampleWindow : public QMainWindow
{
    Q_OBJECT

signals:
/**
 * Indicates the window has been closed
 */
    void closed();
public:
    explicit ApplicationExampleWindow(QWidget *parent = nullptr) : QMainWindow(parent){};
    ~ApplicationExampleWindow(){};

/**
 * Handles a recognition of a gesture.
 * @param gestureID The ID of the recognized gesture
 */

    virtual void handleGestureRecognized(string &gestureID) = 0;

private:
/**
 * Override the QMainWindow CloseEvent emit the closed() signal.
 * Not to be called manually.
 */
    void closeEvent(QCloseEvent *event) override;
};


#endif // APPLICATIONEXAMPLEWINDOW_H
