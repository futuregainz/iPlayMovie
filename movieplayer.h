#ifndef MOVIEPLAYER_H
#define MOVIEPLAYER_H

#include <QWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QDir>
#include <QMediaPlaylist>
#include <QKeyEvent>
#include <QFileDialog>
#include <QResizeEvent>

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
    //void updatePlaylist(int index);
    void displayVideoDuration(qint64 length, qint64 num);

private slots:
    void playSelectedItem(QListWidgetItem* item);
    void setVideoVolume(int vol);
    void getVideoDuration(qint64 length);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    //void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MoviePlayer *ui;

    QMediaPlayer* m_mediaPlayer;
    QMediaPlaylist *playList;
    QVideoWidget *videoWidget;

    PlayListItems *itmes;
    QString dirName;
    QSettings *settings = nullptr;

    void saveVideoSettings(qint64 action, const QString &key);
};

#endif // MOVIEPLAYER_H
