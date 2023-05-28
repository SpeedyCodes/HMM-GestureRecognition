#include "GestureEditor.h"
#include "ui_GestureEditor.h"
#include <QFileDialog>
#include <QStandardPaths>

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
    std::map<std::string, bool> gestureFeatures;
    int vidCount = trainingVideoPaths.size();
    ui->statusLabel->setText("Detecting landmarks...\n");
    for(int i = 0; i < vidCount; i++){
        QString& path = trainingVideoPaths[i];
        std::string str = path.toStdString();
        const char* p = str.c_str();
        data.push_back(mediapipe->getLandmarksFromVideo(p));
        ui->progressBar->setValue(95*(float)(i+1)/vidCount); // roughly 95% of time is used by getLandmarks, TODO remove magic number
    }
    ui->statusLabel->setText(ui->statusLabel->text() + "Landmarks detected\n");
    ui->statusLabel->setText("Detecting gesture features...\n");
    gestureFeatures = MediapipeInterface::getFiltersFromData(data);
    ui->statusLabel->setText("Gesture features detected\n");
    ui->statusLabel->setText(ui->statusLabel->text() + "Preprocessing data\n");
    std::vector<std::vector<Observable>> observables = MediapipeInterface::preprocessData(data);
    ui->statusLabel->setText(ui->statusLabel->text() + "Preprocessed data\n");
    ui->statusLabel->setText(ui->statusLabel->text() + "Fitting HMM\n");
    std::string gestureID = ui->nameLineEdit->text().toStdString();
    library->addGesture(gestureID, gestureFeatures);
    bool success = library->fitAndSelect(observables, gestureID); //TODO stateAmount staat nu standaard op 10, mogelijks aanpassen
    ui->progressBar->setValue(100);
    ui->statusLabel->setText(ui->statusLabel->text() + "HMM fitted\n");
    if(success) close(); // return to the mainwindow if there are no errors to show
}

