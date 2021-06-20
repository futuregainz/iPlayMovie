#ifndef MOVIEplayer_H
#define MOVIEplayer_H

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
class Movieplayer;
}

class Movieplayer : public QWidget
{
    Q_OBJECT

    const int seekPostion = 10000; //seek 10 seconds back or foward;

public:
    explicit Movieplayer(QWidget *parent = 0);
    ~Movieplayer();

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
    Ui::Movieplayer *ui;

    QMediaPlayer *m_mediaplayer;
    QMediaPlaylist *playList;
    QVideoWidget *videoWidget;

    PlayListItems *itmes;
    QString dirName;
    QSettings *settings = nullptr;

    bool resumeVideo(const int &index);
};

#endif // MOVIEplayer_H
