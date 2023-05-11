#include "SignLanguageWriter.h"
#include "ui_SignLanguageWriter.h"

SignLanguageWriter::SignLanguageWriter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SignLanguageWriter)
{
    ui->setupUi(this);
}

SignLanguageWriter::~SignLanguageWriter()
{
    delete ui;
}
