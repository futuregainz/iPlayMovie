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

namespace Ui {
class Movieplayer;
}

class Movieplayer : public QWidget
{
    Q_OBJECT

    const int seekPostion = 15000; //seek 15 seconds back or foward;

public:
    explicit Movieplayer(QWidget *parent = 0);
    ~Movieplayer();

    void loadMediaPlaylist(const QString &mediaPath);
    void isMediaAvailable(bool found);
    void removeCurrentVideo();
    void renameVideo();
    void reloadContent();
    int retry = 0;

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
    //void saveSettings(const int &action, const QString &key, const QVariant &value = 0);

private slots:
    void playSelectedItem(QListWidgetItem* item);
    void setVideoVolume(int vol);
    void getVideoDuration(qint64 length);
    void videoSliderMoved(int value);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::Movieplayer *ui;

    QMediaPlayer *m_mediaplayer = nullptr;
    QMediaPlaylist *playList = nullptr;
    QVideoWidget *videoWidget = nullptr;

    PlayListItems *itmes = nullptr;
    VideoControls *controls = nullptr;

    QString dirName;

    void resumeVideo(int index, bool first = false);
};

#endif // MOVIEplayer_H
