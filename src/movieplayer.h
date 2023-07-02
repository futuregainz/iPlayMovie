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
#include <QMediaMetaData>

#include "playlistitems.h"
#include "videocontrols.h"

class Movieplayer : public QVideoWidget
{
    Q_OBJECT

    const int SEEKPOS = 15000; //seek 15 seconds back or foward;

public:
    explicit Movieplayer(QVideoWidget *parent = 0);
    ~Movieplayer();

    void loadMediaPlaylist(const QString &mediaPath);
    void isMediaAvailable(bool found);
    void removeCurrentVideo();
    void renameVideo();
    void reloadContent();
    int getCurrentIndex(QString name);
    QString getCurrentFilename(int index);

    QFileInfoList videoList;

public slots:
    void playPause();
    void gotoPrevious();
    void gotoNext();

signals:
    bool closeApp();
    void resizeWindow(bool resize);
    void addPlayList(QString item);
    void displayVideoDuration(qint64 length, qint64 num);
    void videoPlaying(bool isPaused);
    //void saveSettings(const int &action, const QString &key, const QVariant &value = 0);
    void volumeChangedViaShortcuts(int val);

private slots:
    void playSelectedItem(QListWidgetItem* item);
    void setVideoVolume(int vol);
    void getVideoDuration(qint64 length);
    void videoSliderMoved(int value);
    void muteVideo();
    //void controlMove();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    //void mouseReleaseEvent(QMouseEvent *event) override;
    //void moveEvent(QMoveEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QMediaPlayer *m_mediaplayer = nullptr;
    QMediaPlaylist *playList = nullptr;
    //QVideoWidget *videoWidget = nullptr;

    PlayListItems *itmes = nullptr;
    VideoControls *controls = nullptr;

    QString dirName;

    void resumeVideo(int index, bool first = false);
};

#endif // MOVIEplayer_H
