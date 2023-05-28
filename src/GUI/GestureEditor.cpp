#include "GestureEditor.h"
#include "ui_GestureEditor.h"
#include <QFileDialog>
#include <QStandardPaths>

#define GET_LANDMARKS_TIME_USED 90.0  // roughly 90% of training time is used by getLandmarks

GestureEditor::GestureEditor(QWidget *parent, GestureLibrary* library, MediapipeInterface* mediapipe) :
    QDialog(parent), ui(new Ui::GestureEditor), library(library), mediapipe(mediapipe)
{
    ui->setupUi(this);
    assert(library != nullptr);
    assert(mediapipe!= nullptr);
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
    int vidCount = trainingVideoPaths.size();
    ui->statusLabel->setText("Detecting landmarks...\n");
    for(int i = 0; i < vidCount; i++){
        QString& path = trainingVideoPaths[i];
        std::string str = path.toStdString();
        const char* p = str.c_str();
        data.push_back(mediapipe->getLandmarksFromVideo(p));
        ui->progressBar->setValue(GET_LANDMARKS_TIME_USED*(float)(i+1)/vidCount);
    }
    ui->statusLabel->setText(ui->statusLabel->text() + "Landmarks detected\n");
    ui->statusLabel->setText(ui->statusLabel->text() + "Preprocessing data\n");
    std::vector<std::vector<Observable>> observables = MediapipeInterface::preprocessData(data);
    ui->statusLabel->setText(ui->statusLabel->text() + "Preprocessed data\n");
    ui->statusLabel->setText(ui->statusLabel->text() + "Fitting HMM\n");
    std::string gestureID = ui->nameLineEdit->text().toStdString();
    library->addGesture(gestureID);
    bool success = library->fitAndSelect(observables, gestureID);
    ui->progressBar->setValue(100);
    ui->statusLabel->setText(ui->statusLabel->text() + "HMM fitted\n");
    if(success) close(); // return to the mainwindow if there are no errors to show
}

