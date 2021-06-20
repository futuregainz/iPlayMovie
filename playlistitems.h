#ifndef PLAYLISTITEMS_H
#define PLAYLISTITEMS_H

#include <QWidget>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QTime>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

namespace Ui {
class PlayListItems;
}

class PlayListItems : public QWidget
{
    Q_OBJECT


public:
    explicit PlayListItems(QWidget *parent = 0);
    ~PlayListItems();

    int lastSavedVolume();
    int getSelectedItem();
    QString getSelectedItemName();
    QString getVideoTime(const int &miliseconds);
    QString prependZero(int val);

    void resetPlaylist();

    int lastSavedVol = 0;

public slots:
    void addPlayListItems(QString item);
    void updateList(int index);
    void removeListItem();
    void displayPlayTime(qint64 length, qint64 num);

signals:
    void newItemSelected(QListWidgetItem*);
    //void closeAllWindows();
    void changeVolume(int value);

private slots:
    void on_volCtrl_valueChanged(int value);
    void on_hide_Button_clicked();

private:
    Ui::PlayListItems *ui;
    void closeEvent(QCloseEvent *bar);
};

#endif // PLAYLISTITEMS_H
