#ifndef GESTUREEDITOR_H
#define GESTUREEDITOR_H

#include <QDialog>
#include "../GestureLibrary.h"

namespace Ui {
class GestureEditor;
}

class GestureEditor : public QDialog
{
    Q_OBJECT

public:
    explicit GestureEditor(QWidget *parent = nullptr, GestureLibrary* library=nullptr);
    ~GestureEditor();

private slots:
    void on_selectVideoButton_clicked();

    void on_trainButton_clicked();

private:
    Ui::GestureEditor *ui;
    GestureLibrary* library;
    QStringList trainingVideoPaths;
};

#endif // GESTUREEDITOR_H
