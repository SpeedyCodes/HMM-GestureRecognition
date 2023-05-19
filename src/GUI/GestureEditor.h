#ifndef GESTUREEDITOR_H
#define GESTUREEDITOR_H

#include <QDialog>
#include "../GestureLibrary.h"
#include "src/utils/MediapipeInterface.h"

namespace Ui {
class GestureEditor;
}

class GestureEditor : public QDialog
{
    Q_OBJECT

public:
    explicit GestureEditor(QWidget *parent = nullptr, GestureLibrary* library=nullptr, MediapipeInterface* mediapipe=nullptr);
    ~GestureEditor();

private slots:
    void on_selectVideoButton_clicked();

    void on_trainButton_clicked();

private:
    Ui::GestureEditor *ui;
    GestureLibrary* library;
    QStringList trainingVideoPaths;
    MediapipeInterface* mediapipe;
};

#endif // GESTUREEDITOR_H
