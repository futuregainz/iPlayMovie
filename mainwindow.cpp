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
    connect(m_player, SIGNAL(resizeWindow()), this, SLOT(setMinimumSize()));
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
    _loginClass->hide();
    m_player->videoStart();

    this->installEventFilter(m_player);
    this->setFocus();
}

void MainWindow::setMinimumSize()
{
    if(!this->isFullScreen()) { this->showFullScreen();
    }
    else { this->showNormal(); }
}