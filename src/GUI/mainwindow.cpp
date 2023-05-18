#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QLayout>
#include <thread>
#include <QObject>
#include <QThread>

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

    signLanguageWriter = nullptr;
    robotConnectionManager = nullptr;

    ui->availableGestureListWidget->addItem("Een");
    ui->availableGestureListWidget->addItem("Twee");
    ui->availableGestureListWidget->addItem("Drie");
    ui->availableGestureListWidget->addItem("Vier");

    mediaPipeInterface.open(); // Open tcp thread

    QObject::connect(&mediaPipeInterface, &MediapipeInterface::imageAvailable,
                this, &MainWindow::paintRealtimeFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_Gesture_triggered()
{
    GestureEditor* editor = new GestureEditor(this);
    editor->setModal(true);
    editor->exec();
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

