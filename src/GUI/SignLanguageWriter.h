#ifndef SIGNLANGUAGEWRITER_H
#define SIGNLANGUAGEWRITER_H

#include <QMainWindow>
#include "ApplicationExampleWindow.h"

namespace Ui {
class SignLanguageWriter;
}

class SignLanguageWriter : public ApplicationExampleWindow
{
    Q_OBJECT

public:
    explicit SignLanguageWriter(QWidget *parent = nullptr);
    ~SignLanguageWriter();
    void handleGestureRecognized(string &gestureID) override;

private:
    Ui::SignLanguageWriter *ui;
    QString text;
};

#endif // SIGNLANGUAGEWRITER_H
