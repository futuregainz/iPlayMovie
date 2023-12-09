#ifndef PLAYLISTITEMS_H
#define PLAYLISTITEMS_H

#include <QWidget>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include "text.h"
#include "QtCore/qobject.h"
#include <QUrl>
#include <QVector>


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
    void playlistClear();

    //void removeItem(int row);
    //void renameItem(int row, QString const &newName);
    int lastSavedVol = 0;

    int currentIndex();
    void setCurrentIndex(int index);
    void setCurrentIndex(const QString &vidName);
    void addMedia(const QString &item);
    int previousIndex();
    int nextIndex();
    QString currentMedia();
    QString lastMedia();
    //QString getVideoName(const int &iIndex);
    //void removeMedia(const int &index);
    //void renameMedia(const int &index, const QString &newName);

public slots:
    void addPlayListItem(QString item);
    //void updateList(int index);

signals:
    void newItemSelected(QListWidgetItem*);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void searchForVid();
    void on_lineEdit_textEdited(const QString &arg1);

protected:
    void closeEvent(QCloseEvent *bar) override;

private:
    Ui::PlayListItems *ui;
    QStringList ui_playList;
    QVector<QString> playlist;
    QString lastPlayed;
    int index;
};

#endif // PLAYLISTITEMS_H
