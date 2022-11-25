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

public slots:
    void saveSettings(const int &action, const QString &key, const QVariant &value = 0);

signals:
    void closeWindows();

private slots:
    void userDataValid();
    void setMinimumSize(bool resize);

protected:
    void closeEvent(QCloseEvent *bar) override;

private:
    Ui::MainWindow *ui;

    HandleLoging *_loginClass = nullptr;
    Movieplayer *m_player = nullptr;
};

#endif // MAINWINDOW_H
