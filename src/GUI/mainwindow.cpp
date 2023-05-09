#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QStringListModel>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QLayout>

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
    connect(&server, &QTcpServer::newConnection, this, &MainWindow::acceptConnection);
    server.listen(QHostAddress::Any, 5000);
    // Running the python server automagically
//    std::string filename = "../src/MediapipeInterface/main.py";
//    std::string command = "python ";
//    command += filename;
//    system(command.c_str());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::acceptConnection()
{
    connection = server.nextPendingConnection();
    if (!connection) {
        qDebug() << "Error: got invalid pending connection!";
        return;
    }

    connect(connection, &QIODevice::readyRead,
            this, &MainWindow::onReadyRead);

    qDebug() << "Accepted connection";
    server.close();
}

void MainWindow::onReadyRead()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray data = sender->readAll();
    //QImage img((uchar*)data.data(), 1920, 1080, QImage::Format_Grayscale16);
    QImage image;
    image.loadFromData(data);
    ui->label_4->setPixmap(QPixmap::fromImage(image));
    //socket->write(QByteArray::fromStdString(sender->peerAddress().toString().toStdString() + ": " + datas.toStdString()));
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

