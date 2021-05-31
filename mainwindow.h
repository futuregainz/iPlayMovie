#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "handleloging.h"
#include "movieplayer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void userDataValid();
    void setMinimumSize();

private:
    Ui::MainWindow *ui;

    HandleLoging *_loginClass;
    MoviePlayer *m_player;
};

#endif // MAINWINDOW_H
