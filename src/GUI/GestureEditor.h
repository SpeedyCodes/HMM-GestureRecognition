#ifndef GESTUREEDITOR_H
#define GESTUREEDITOR_H

#include <QDialog>

namespace Ui {
class GestureEditor;
}

class GestureEditor : public QDialog
{
    Q_OBJECT

public:
    explicit GestureEditor(QWidget *parent = nullptr);
    ~GestureEditor();

private:
    Ui::GestureEditor *ui;
};

#endif // GESTUREEDITOR_H
