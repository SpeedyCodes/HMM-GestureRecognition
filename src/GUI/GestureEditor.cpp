#include "GestureEditor.h"
#include "ui_GestureEditor.h"

GestureEditor::GestureEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GestureEditor)
{
    ui->setupUi(this);
}

GestureEditor::~GestureEditor()
{
    delete ui;
}
