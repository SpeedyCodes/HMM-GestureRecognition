
#include "ApplicationExampleWindow.h"

void ApplicationExampleWindow::closeEvent(QCloseEvent *event) {
    emit closed();
}