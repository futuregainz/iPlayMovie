#ifndef MOVIEplayer_H
#define MOVIEplayer_H

#include <QVideoWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QDir>
#include <QKeyEvent>
#include <QFileDialog>
#include <QResizeEvent>
#include <QMediaMetaData>
#include <QMap>
#include "playlistitems.h"
#include "videocontrols.h"

class Movieplayer : public QVideoWidget
{
    Q_OBJECT

    const int SEEKPOS = 10000; //seek 10 seconds back or foward;

public:
    explicit Movieplayer(QVideoWidget *parent = 0);
    ~Movieplayer();

    void loadMediaPlaylist(const QString &mediaPath);
    void isMediaAvailable(bool found);

    QFileInfoList videoList;

public slots:
    void playPause();
    void gotoPrevious();
    void gotoNext();

signals:
    bool closeApp();
    void resizeWindow(bool resize);
    void addPlayList(const QString &item);
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
    void resumeVideoPosition(QMediaPlayer::MediaStatus status);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    //void mouseReleaseEvent(QMouseEvent *event) override;
    //void moveEvent(QMoveEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QMediaPlayer *m_mediaplayer = nullptr;
    QAudioOutput  audioOutput;
    PlayListItems *playList = nullptr;
    VideoControls *controls = nullptr;

    QString dirName;
    bool videoNameTaken(const QString &vidName);
    void removeCurrentVideo();
    void renameVideo();
    void resumeVideo(bool first = false);
    void playVideo(int index);
    qint64 pos = 0;
};

#endif // MOVIEplayer_H
