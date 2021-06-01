#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _loginClass(new HandleLoging),
    m_player(new MoviePlayer)
{
    ui->setupUi(this);
    ui->mainLayout->addWidget(_loginClass);

    connect(_loginClass, SIGNAL(userLoginValid()), this, SLOT(userDataValid()));
    connect(m_player, SIGNAL(closeApp()), this, SLOT(close()));
    connect(m_player, SIGNAL(resizeWindow(bool)), this, SLOT(setMinimumSize(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
    if (_loginClass) delete _loginClass;
    if (m_player) delete m_player;
}

void MainWindow::userDataValid()
{
    ui->mainLayout->replaceWidget(_loginClass, m_player);
    _loginClass->close();
    m_player->loadMediaPlaylist(QString(VIDEO_DIR));

    this->installEventFilter(m_player);
    this->setFocus();
}

void MainWindow::setMinimumSize(bool resize)
{
    if(resize) {

        if(!this->isFullScreen()) {
            this->showFullScreen();
        }
        else{
            this->showNormal();
        }
    }
    else {
        this->showNormal();
    }
}

void MainWindow::closeEvent(QCloseEvent *bar)
{
    emit m_player->closeApp();
    bar->accept();
}
