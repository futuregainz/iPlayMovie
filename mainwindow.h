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

signals:
    void closeWindows();
private slots:
    void userDataValid();
    void setMinimumSize(bool resize);

private:
    Ui::MainWindow *ui;

    HandleLoging *_loginClass;
    MoviePlayer *m_player;

    void closeEvent(QCloseEvent *bar);
};

#endif // MAINWINDOW_H
