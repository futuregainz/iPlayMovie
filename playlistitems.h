#ifndef PLAYLISTITEMS_H
#define PLAYLISTITEMS_H

#include <QWidget>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include "text.h"

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
    QString getItemName();
    QString getItemName(int index);

    void resetPlaylist();

    int lastSavedVol = 0;

public slots:
    void addPlayListItems(QString item);
    void updateList(int index);

signals:
    void newItemSelected(QListWidgetItem*);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

protected:
    void closeEvent(QCloseEvent *bar) override;

private:
    Ui::PlayListItems *ui;

};

#endif // PLAYLISTITEMS_H
