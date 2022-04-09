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
    void addPlayListItem(QString item);
    void updateList(int index);

signals:
    void newItemSelected(QListWidgetItem*);
    void reloadPlaylist();

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_searchButton_clicked();

    void on_lineEdit_textEdited(const QString &arg1);

protected:
    void closeEvent(QCloseEvent *bar) override;

private:
    Ui::PlayListItems *ui;

    QStringList playList;

};

#endif // PLAYLISTITEMS_H
