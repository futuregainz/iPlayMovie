#ifndef MOVIEPLAYER_H
#define MOVIEPLAYER_H

#include <QWidget>
#include <QDebug>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QDir>
#include <QMediaPlaylist>
#include <QKeyEvent>
#include <QMessageBox>
#include <QFileDialog>

#include "playlistitems.h"

namespace Ui {
class MoviePlayer;
}

class MoviePlayer : public QWidget
{
    Q_OBJECT

    const int seekPostion = 30000; //seek 30 seconds back or foward;

public:
    explicit MoviePlayer(QWidget *parent = 0);
    ~MoviePlayer();

    void loadMediaPlaylist(const QString &mediaPath);
    void isMediaAvailable(bool found);
    void removeCurrentVideo();

signals:
    bool closeApp();
    void resizeWindow(bool resize);
    void addPlayList(QString item);
    void removeListItem();

private slots:
    void playSelectedItem(QListWidgetItem* item);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Ui::MoviePlayer *ui;

    QMediaPlayer* m_mediaPlayer;
    QMediaPlaylist *playList;
    QVideoWidget *videoWidget;

    PlayListItems *itmes;

    QString dirName;
};

#endif // MOVIEPLAYER_H
