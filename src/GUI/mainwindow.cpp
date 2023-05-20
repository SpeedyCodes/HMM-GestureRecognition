#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QLayout>
#include <thread>
#include <QObject>
#include <QStandardPaths>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Potentially useful for reading from a video file later
//    QMediaPlayer* player = new QMediaPlayer(this);
//    QVideoWidget *videoWidget = new QVideoWidget;

//    QBoxLayout *layout = new QVBoxLayout;
//    layout->addWidget(videoWidget);

    //ui->videoFrame->setLayout(layout);

    //player->setVideoOutput(videoWidget);
    //player->setSource(QUrl("D:/School/UAntwerpen/TA/TOg/HMM-GestureRecognition/cmake-build-debug-mingw/PAL-CLIP PP4.mp4"));
    //player->play();
    //videoWidget->show();
    library = new GestureLibrary();
    signLanguageWriter = nullptr;
    robotConnectionManager = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_Gesture_triggered()
{
    GestureEditor* editor = new GestureEditor(this, library, &mediaPipeInterface);
    editor->setModal(true);
    editor->exec();
    delete(editor);
    refreshGesturesView();
}


void MainWindow::on_actionSign_Language_triggered()
{
    if(signLanguageWriter != nullptr){
        signLanguageWriter->setWindowState(Qt::WindowState::WindowActive);
    }else{
        signLanguageWriter = new SignLanguageWriter(this);
        signLanguageWriter->show();
    }

}


void MainWindow::on_actionRobot_triggered()
{
    if(robotConnectionManager != nullptr){
        robotConnectionManager->setWindowState(Qt::WindowState::WindowActive);
    }else{
        robotConnectionManager = new RobotConnectionManager(this);
        robotConnectionManager->show();
    }
}

void MainWindow::paintRealtimeFrame(QImage& image) {
    ui->label_4->setPixmap(QPixmap::fromImage(image));
}


void MainWindow::on_cameraToggle_clicked()
{
    if(!mediaPipeInterface.isOpen()){
        mediaPipeInterface.open(); // Open tcp thread
        QObject::connect(&mediaPipeInterface, &MediapipeInterface::imageAvailable,
                         this, &MainWindow::paintRealtimeFrame);
    }else{
        QObject::disconnect(&mediaPipeInterface, &MediapipeInterface::imageAvailable,
                         this, &MainWindow::paintRealtimeFrame);
        QImage black;
        black.fill(Qt::black);
        paintRealtimeFrame(black);
        mediaPipeInterface.close();
    }
}

void MainWindow::refreshGesturesView() {
    ui->gesturesListWidget->clear();
    const map<string, Gesture>& gestures = library->getGestures();
    for(auto it = gestures.begin(); it != gestures.end(); it++){
        ui->gesturesListWidget->addItem(QString::fromStdString(it->first));
    }
}


void MainWindow::on_actionSave_all_Gestures_triggered()
{
    if(!library->isFileSystemInitiated()){
        QString path = QFileDialog::getSaveFileName(this, tr("Save Gesture Library to file"),
                                      QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
                                      tr("Gesture Libraries (*.gesturelibrary)"));
        if(path == "") return;
        library->initiateFileSystem(path.toStdString());
    }
    library->updateSavedGestures();
}

void MainWindow::on_actionLoad_Gesture_Library_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Load Gesture Library"),
                                 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                 tr("Gesture Libraries (*.gesturelibrary)"));
    library->readIn(path.toStdString());
    refreshGesturesView();
}

