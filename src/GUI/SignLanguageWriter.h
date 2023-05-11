#ifndef SIGNLANGUAGEWRITER_H
#define SIGNLANGUAGEWRITER_H

#include <QMainWindow>

namespace Ui {
class SignLanguageWriter;
}

class SignLanguageWriter : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignLanguageWriter(QWidget *parent = nullptr);
    ~SignLanguageWriter();

private:
    Ui::SignLanguageWriter *ui;
};

#endif // SIGNLANGUAGEWRITER_H
