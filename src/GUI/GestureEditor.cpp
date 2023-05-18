#include "GestureEditor.h"
#include "ui_GestureEditor.h"
#include <QFileDialog>
#include <QStandardPaths>
#include "src/utils/MediapipeInterface.h"

GestureEditor::GestureEditor(QWidget *parent, GestureLibrary* library) :
    QDialog(parent), ui(new Ui::GestureEditor), library(library)
{
    ui->setupUi(this);
}

GestureEditor::~GestureEditor()
{
    delete ui;
}

void GestureEditor::on_selectVideoButton_clicked()
{
    trainingVideoPaths = QFileDialog::getOpenFileNames(this, tr("Select videos to train on"),
                                                       QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
                                                       tr("Video Files (*.mp4 *.avi *.mov)"));
    for(QString& path: trainingVideoPaths){
        ui->selectedVideos->addItem(path);
    }

}


void GestureEditor::on_trainButton_clicked()
{
    std::vector<std::vector<std::vector<double>>> data;
    for(QString path: trainingVideoPaths){
        std::string str = path.toStdString();
        const char* p = str.c_str();
        data.push_back(MediapipeInterface::getLandmarksFromVideo(p));
        qDebug() << path << " read out";
    }
    ui->statusLabel->setText(ui->statusLabel->text() + "Landmarks detected\n");
    std::vector<std::vector<Observable>> observables = MediapipeInterface::preprocessData(data);
    ui->statusLabel->setText(ui->statusLabel->text() + "Preprocessed data\n");
    std::string gestureID = ui->nameLineEdit->text().toStdString();
    library->addGesture(gestureID);
    library->fitAndSelect(observables, gestureID);
    ui->statusLabel->setText(ui->statusLabel->text() + "fitted\n");
}

