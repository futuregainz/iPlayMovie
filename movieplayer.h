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

     void videoStart();
    //void removeCurrentVideo(int index);

signals:
     bool closeApp();
     void resizeWindow();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Ui::MoviePlayer *ui;

    QMediaPlayer* m_mediaPlayer;
    QMediaPlaylist *playList;
    QVideoWidget *videoWidget;
};

#endif // MOVIEPLAYER_H
