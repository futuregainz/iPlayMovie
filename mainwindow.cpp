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
    connect(this, SIGNAL(closeWindows()), m_player, SIGNAL(closeApp()));
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
    int exec = QMessageBox::question(this, QString("Exit iPlay Movie?"), QString("\nClick yes to confirm."),
                                    QMessageBox::Yes | QMessageBox::No);
    if(exec == QMessageBox::No)
    {
        bar->ignore();
        return;
    }

    emit closeWindows();
    bar->accept();
}
