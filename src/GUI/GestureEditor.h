#ifndef GESTUREEDITOR_H
#define GESTUREEDITOR_H

#include <QDialog>
#include "../GestureLibrary.h"
#include "src/utils/MediapipeInterface.h"

namespace Ui {
class GestureEditor;
}

/**
 * The window for training new gestures from videos.
 */
class GestureEditor : public QDialog
{
    Q_OBJECT

public:
/**
 * Despite the default arguments, all parameters must be passed in
 * @param parent The parent window
 * @param library The library to add new gestures to
 * @param mediapipe Used to extract data from videos.
 */
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
