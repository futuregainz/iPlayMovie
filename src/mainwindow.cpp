#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _loginClass(new HandleLoging),
    m_player(new Movieplayer)
{
    ui->setupUi(this);
    this->setCentralWidget(_loginClass);

    connect(_loginClass, SIGNAL(userLoginValid()), this, SLOT(userDataValid()));
    connect(m_player, SIGNAL(closeApp()), this, SLOT(close()));
    connect(m_player, SIGNAL(resizeWindow(bool)), this, SLOT(setMinimumSize(bool)));
    connect(this, SIGNAL(closeWindows()), m_player, SIGNAL(closeApp()));

    saveSettings(restore,"mainwindow/geometry");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::userDataValid()
{
    m_player->loadMediaPlaylist(QDir::homePath() + QString(VIDEO_DIR));
    //m_player->layout()->setSizeConstraint(QLayout::SetDefaultConstraint);
    this->setCentralWidget(m_player);

    this->installEventFilter(m_player);
    this->setFocus();
}

void MainWindow::setMinimumSize(bool resize)
{
    if (resize && !this->isFullScreen())
        this->showFullScreen();
    else
        this->showNormal();
}

void MainWindow::closeEvent(QCloseEvent *bar)
{
    if (confirmed || QMessageBox::question(this, "", "Exit iPlay Movie?\nClick yes to confirm.",
                                     QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        bar->ignore();
        return;
    }

    confirmed = true;
    emit closeWindows();
    saveSettings(save, "mainwindow/geometry", saveGeometry());

    QWidget::closeEvent(bar);
}

void MainWindow::saveSettings(const int &action, const QString &key, const QVariant &value)
{
    QSettings settings(COMPANY, APPNAME);

    switch (action)
    {
    case save:
        settings.setValue(key, value);
        break;
    case restore:
        this->restoreGeometry(settings.value(key).toByteArray());
        break;
    default:
        break;
    }
}
