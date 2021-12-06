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

    enum action {save, restore};

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool confirmed = false;

signals:
    void closeWindows();

private slots:
    void userDataValid();
    void setMinimumSize(bool resize);
    void updateWindowTitle(QString name);

protected:
    void closeEvent(QCloseEvent *bar) override;

private:
    Ui::MainWindow *ui;

    HandleLoging *_loginClass;
    Movieplayer *m_player;

    void saveSettings(const int &action);
};

#endif // MAINWINDOW_H
