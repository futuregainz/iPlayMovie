#include "movieplayer.h"
#include "ui_movieplayer.h"
#include "text.h"

MoviePlayer::MoviePlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoviePlayer)
{
    ui->setupUi(this);

    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    videoWidget = new QVideoWidget(this);
    playList = new QMediaPlaylist(this);

    m_mediaPlayer->setVideoOutput(videoWidget);
    ui->videoLayout->addWidget(videoWidget);
}

MoviePlayer::~MoviePlayer()
{
    delete ui;
}

void MoviePlayer::videoStart()
{
    if(system("ls /Users/moegainz/Downloads/ | grep -q '.mp4' > /dev/null") == 0)
        system("mv /Users/moegainz/Downloads/*.mp4 /Users/moegainz/.metadata/");


    QFileInfoList videoList = QDir(QString(VIDEO_DIR)).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(QFileInfo entry : videoList)
    {
        playList->addMedia(QUrl::fromLocalFile(QString(VIDEO_DIR) + entry.fileName()));
    }

    playList->setPlaybackMode(QMediaPlaylist::Loop);
    playList->shuffle();

    m_mediaPlayer->setPlaylist(playList);
    m_mediaPlayer->play();
}

/*void MoviePlayer::removeCurrentVideo(int index)
{
    QFile removeVid(QString(VIDEO_DIR) + videoName);

    if(removeVid.exists()) {

        int exec = QMessageBox::question(nullptr, tr("Delete file?"),
                              tr("Are you sure you want to delete current video\n"
                                 "Click yes to confirm."), QMessageBox::Yes | QMessageBox::No);

        if(exec == QMessageBox::Yes) removeVid.remove();
    }
}*/

bool MoviePlayer::eventFilter(QObject *obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {

        QKeyEvent *keyPress = static_cast<QKeyEvent *>(event);

        if(keyPress->key() == Qt::Key_Right) {

            m_mediaPlayer->setPosition(m_mediaPlayer->position() +  seekPostion);
        }
        else if(keyPress->key() == Qt::Key_Left) {

            m_mediaPlayer->setPosition(m_mediaPlayer->position() - seekPostion);
        }
        /*else if (keyPress->key() == Qt::Key_Space) {

            if (m_mediaPlayer->state() == QMediaPlayer::PausedState) {
                m_mediaPlayer->play();
            }
            else {m_mediaPlayer->pause(); }
        }*/
        else if (keyPress->key() == Qt::Key_Up) {

            playList->next();
        }
        else if (keyPress->key() == Qt::Key_Down) {

            playList->previous();
        }
        else if (keyPress->key() == Qt::Key_M) {

            (!m_mediaPlayer->isMuted())? m_mediaPlayer->setMuted(true) : m_mediaPlayer->setMuted(false);
        }
        else if (keyPress->key() == Qt::Key_X) {

            emit closeApp();
        }
        else if (keyPress->key() == Qt::Key_Escape || keyPress->key() == Qt::Key_F) {

            emit resizeWindow();
        }
        /*else if (keyPress->key() == Qt::Key_D) {

            removeCurrentVideo(playList->currentIndex());
        }*/
        else {

            return false;
        }

        return true;
    }
    else
    {
        event->ignore();
        return false;
    }

    return QWidget::eventFilter(obj, event);
}