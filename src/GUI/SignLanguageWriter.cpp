#include "SignLanguageWriter.h"
#include "ui_SignLanguageWriter.h"

SignLanguageWriter::SignLanguageWriter(QWidget *parent) : ApplicationExampleWindow(parent), ui(new Ui::SignLanguageWriter)
{
    ui->setupUi(this);
}

SignLanguageWriter::~SignLanguageWriter()
{
    delete ui;
}

void SignLanguageWriter::handleGestureRecognized(string &gestureID) {
    text += " " + gestureID;
    ui->textEdit->setText(text);
}
