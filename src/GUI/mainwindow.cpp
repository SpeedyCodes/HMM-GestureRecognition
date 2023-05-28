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
#include <QCheckBox>
#include <QKeyEvent>

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

    // Create a checkbox
    QCheckBox* checkBox = new QCheckBox("Multiple features on", this);
    QObject::connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(setMultiple(bool)));
    QVBoxLayout* layout = new QVBoxLayout(ui->centralWidget);
    layout->setContentsMargins(20, 480, 0, 0);
    layout->addWidget(checkBox);

    // Create a checkbox2
    QCheckBox* checkBox2 = new QCheckBox("Use Q to segment realtime feed", this);
    checkBox2->setChecked(true);
    QObject::connect(checkBox2, SIGNAL(clicked(bool)), this, SLOT(setAllowFilter(bool)));
    //QVBoxLayout* layout2 = new QVBoxLayout(ui->centralWidget);
    layout->setContentsMargins(20, 520, 0, 0);
    layout->addWidget(checkBox2);

    QIcon icon("src/GUI/logo.png");
    this->setWindowIcon(icon);
    // Install an event filter on the application to capture key press events
    qApp->installEventFilter(this);

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
        QObject::connect(&mediaPipeInterface, &MediapipeInterface::dataAvailable,
                         this, &MainWindow::tryRealtimeRecognition);
    }else{
        QObject::disconnect(&mediaPipeInterface, &MediapipeInterface::imageAvailable,
                         this, &MainWindow::paintRealtimeFrame);
        QObject::disconnect(&mediaPipeInterface, &MediapipeInterface::dataAvailable,
                            this, &MainWindow::tryRealtimeRecognition);
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


void MainWindow::on_videoPickButton_clicked()
{
    videoInputPath = QFileDialog::getOpenFileName(this, tr("Select video to recognize gestures in"),
                                                       QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
                                                       tr("Video Files (*.mp4 *.avi *.mov)"));
    if (videoInputPath != ""){
        ui->selectedVideoLabel->setText(videoInputPath);
    }
}


void MainWindow::on_recognizeButton_clicked()
{
    std::string str = videoInputPath.toStdString();
    if(str.empty()) return;
    const char* p = str.c_str();
    string result = library->recognizeFromVideo(p, &mediaPipeInterface);
    handleGestureRecognized(result);
}

void MainWindow::handleGestureRecognized(std::string &gestureID) {
    ui->recognizedListWidget->addItem(QString::fromStdString(gestureID));
}

void MainWindow::tryRealtimeRecognition(const std::vector<double> &landmarks) {
    std::string gesture = library->realtimeRecognition(landmarks);
    if(!gesture.empty()){
        handleGestureRecognized(gesture);
    }
}

void MainWindow::setMultiple(bool check) {
    library->setMultipleOn(check);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // Check if the pressed key is "Q"
        if (keyEvent->key() == Qt::Key_Q)
        {
            // Toggle the bool variable each time the "Q" key is pressed
            library->filterPressed = !library->filterPressed;
            if(!library->filterPressed) library->startAnalysis = true;
            else library->startAnalysis = false;
        }
    }

    // Call the base class implementation for normal event processing
    return QObject::eventFilter(obj, event);
}

void MainWindow::setAllowFilter(bool res){
    library->allowFilter = res;
}
