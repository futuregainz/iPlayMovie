#include "movieplayer.h"

#include <QInputDialog>
#include <QMediaObject>

Movieplayer::Movieplayer(QVideoWidget *parent) :
    QVideoWidget(parent),
    itmes(new PlayListItems())
{
    controls = new VideoControls();

    m_mediaplayer = new QMediaPlayer(0, QMediaPlayer::StreamPlayback);
    playList = new QMediaPlaylist(m_mediaplayer);

    m_mediaplayer->setVideoOutput(this);
    this->setAspectRatioMode(Qt::KeepAspectRatio);

    connect(this, SIGNAL(addPlayList(QString)), itmes, SLOT(addPlayListItem(QString)));
    connect(this, SIGNAL(closeApp()), itmes, SLOT(close()));
    connect(this, SIGNAL(closeApp()), controls, SLOT(close()));
    connect(itmes, SIGNAL(newItemSelected(QListWidgetItem*)), this, SLOT(playSelectedItem(QListWidgetItem*)));
    connect(controls, SIGNAL(videoSliderMoved(int)), this, SLOT(videoSliderMoved(int)));
    connect(m_mediaplayer, SIGNAL(positionChanged(qint64)), this, SLOT(getVideoDuration(qint64)));
    connect(m_mediaplayer, SIGNAL(durationChanged(qint64)), controls, SLOT(setVideoRange(qint64)));
    connect(this, SIGNAL(displayVideoDuration(qint64,qint64)), controls, SLOT(displayVideoDuration(qint64,qint64)));
    connect(this, SIGNAL(videoPlaying(bool)), controls, SLOT(videoPaused(bool)));
    connect(controls, SIGNAL(playPauseVideo()), this, SLOT(playPause()));
    connect(controls, SIGNAL(gotoNextVideo()), this, SLOT(gotoNext()));
    connect(controls, SIGNAL(gotoPreviousVideo()), this, SLOT(gotoPrevious()));
    connect(controls, SIGNAL(changeVolume(int)), this, SLOT(setVideoVolume(int)));
    connect(m_mediaplayer, SIGNAL(mutedChanged(bool)), controls, SLOT(videoMuted(bool)));
    connect(this, SIGNAL(volumeChangedViaShortcuts(int)), controls, SLOT(volumeChanged(int)));
    connect(controls, SIGNAL(muteButtonClicked()), this, SLOT(muteVideo()));
}

Movieplayer::~Movieplayer()
{
    delete itmes;
    delete controls;
}

void Movieplayer::loadMediaPlaylist(const QString &mediaPath)
{
    QString homePaht = QDir::homePath();
    QString getVideo = "mv " + homePaht + QString(DOWNLOADS) + "*.mp4 " +  homePaht + QString(VIDEO_DIR);

    dirName = mediaPath;
    QStringList filters;
    filters << "*.mp4" << "*.mov";

    videoList = QDir(homePaht + QString(DOWNLOADS)).entryInfoList(filters, QDir::AllEntries | QDir::NoDotAndDotDot);

    if (videoList.size() > 0)
        system(getVideo.toLocal8Bit().data());

    videoList = QDir(mediaPath).entryInfoList(filters, QDir::AllEntries | QDir::NoDotAndDotDot);

    isMediaAvailable((videoList.size() > 0));

    int count = 0;

    foreach (QFileInfo entry, videoList)
    {
        QString vidname = entry.fileName();

        if (vidname.contains(" "))
        {
            vidname = vidname.replace(" ", "-");
            QFile::rename(mediaPath + entry.fileName(), mediaPath + vidname);
        }

        playList->addMedia(QUrl::fromLocalFile(mediaPath + vidname));
        playList->setCurrentIndex(count);
        vidMap[playList->currentIndex()] = vidname;
        emit addPlayList(vidname);
        count++;
    }

    //qDebug() << QString("Count:  %1").arg(QString::number(count));

    playList->setPlaybackMode(QMediaPlaylist::Loop);
    m_mediaplayer->setPlaylist(playList);
    setVideoVolume(itmes->lastSavedVolume());
    resumeVideo(0, true);
    itmes->show();
    controls->show();
}

void Movieplayer::isMediaAvailable(bool found)
{
    if (found)
        return;

    int exec = QMessageBox::critical(this, QString("Empty PlayList"),
                                     QString("No valid media (.mp4) was found in '%1'\n Try a different folder?").arg(dirName),
                                     QMessageBox::Yes | QMessageBox::No);

    if (exec == QMessageBox::Yes)
    {
        dirName = QFileDialog::getExistingDirectory(this, tr("Select a Different Folder"),
                                                    dirName,
                                                    QFileDialog::ShowDirsOnly);

        (!dirName.isEmpty())? loadMediaPlaylist(dirName) : isMediaAvailable(false);
    }
    else
    {
        emit closeApp();
    }
}

void Movieplayer::playSelectedItem(QListWidgetItem *item)
{
    int index = getCurrentIndex(item->text());
    resumeVideo(index);
}

void Movieplayer::setVideoVolume(int vol)
{
    if (vol < 0) return;

    m_mediaplayer->setVolume(vol);

    if (m_mediaplayer->isMuted())
        m_mediaplayer->setMuted(false);
}

void Movieplayer::getVideoDuration(qint64 length)
{
    qint64 num = m_mediaplayer->duration();
    length = m_mediaplayer->position();
    emit displayVideoDuration(length, num);
}

void Movieplayer::videoSliderMoved(int value)
{
    m_mediaplayer->setPosition(value);
}

void Movieplayer::muteVideo()
{
    (!m_mediaplayer->isMuted())? m_mediaplayer->setMuted(true) : m_mediaplayer->setMuted(false);
}

void Movieplayer::removeCurrentVideo()
{
    QString videoName = getCurrentFilename(playList->currentIndex());
    QFile removeVid(dirName + "/" + videoName);

    if (!removeVid.exists())
        return;

    if (QMessageBox::question(this, QString("Delete Video?"), QString("Are you sure you want to delete\n%1"
                                                                      "\nClick yes to confirm.").arg(videoName),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) return;

    if (removeVid.remove())
        reloadContent();
}

void Movieplayer::renameVideo()
{
    bool ok = false;
    QString name = getCurrentFilename(playList->currentIndex());
    QString text = QInputDialog::getText(this,  tr("Rename video"),
                                         tr("Enter new name"),
                                         QLineEdit::Normal, name, &ok );

    if (ok && !text.isEmpty() )
    {
        if (!text.endsWith(".mp4"))
            text.append(".mp4");

        if (QFile::rename(dirName + "/" + name, dirName + "/" + text))
            reloadContent();
    }
}

void Movieplayer::reloadContent()
{
    playList->clear();
    itmes->resetPlaylist();
    vidMap.clear();
    loadMediaPlaylist(QDir::homePath() + QString(VIDEO_DIR));
}

void Movieplayer::playPause()
{
    bool isPaused = m_mediaplayer->state() == QMediaPlayer::PausedState;

    if (isPaused)
    {
        resumeVideo(playList->currentIndex());
    }
    else
    {
        m_mediaplayer->pause();
    }

    emit videoPlaying(isPaused);
}

void Movieplayer::gotoPrevious()
{
    resumeVideo(playList->previousIndex());
}

void Movieplayer::gotoNext()
{
    resumeVideo(playList->nextIndex());
}

bool Movieplayer::eventFilter(QObject *obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyPress = static_cast<QKeyEvent *>(event);

        if (keyPress->key() == Qt::Key_Right)
        {
            m_mediaplayer->setPosition(m_mediaplayer->position() +  SEEKPOS);
        }
        else if  (keyPress->key() == Qt::Key_Left)
        {
            m_mediaplayer->setPosition(m_mediaplayer->position() - SEEKPOS);
        }
        else if (keyPress->key() == Qt::Key_Space)
        {
            playPause();
        }
        else if (keyPress->key() == Qt::Key_Up)
        {
            gotoPrevious();
        }
        else if (keyPress->key() == Qt::Key_Down)
        {
            gotoNext();
        }
        else if (keyPress->key() == Qt::Key_U)
        {
            int vol = m_mediaplayer->volume() + 10;
            setVideoVolume(vol);
            emit volumeChangedViaShortcuts(vol);
        }
        else if (keyPress->key() == Qt::Key_Y)
        {
            int vol = m_mediaplayer->volume() - 10;
            setVideoVolume(vol);
            emit volumeChangedViaShortcuts(vol);
        }
        else if (keyPress->key() == Qt::Key_M)
        {
            muteVideo();
        }
        else if (keyPress->key() == Qt::Key_X)
        {
            emit closeApp();
        }
        else if (keyPress->key() == Qt::Key_Escape || keyPress->key() == Qt::Key_F)
        {
            emit resizeWindow(true);
            controls->hide();
            itmes->hide();
        }
        else if (keyPress->key() == Qt::Key_D)
        {
            removeCurrentVideo();
        }
        else if (keyPress->key() == Qt::Key_L )
        {
            if (playList->playbackMode() != QMediaPlaylist::Loop)
            {
                playList->setPlaybackMode(QMediaPlaylist::Loop);
            }
            else
            {
                playList->setPlaybackMode(QMediaPlaylist::Sequential);
            }
        }
        else if (keyPress->key() == Qt::Key_R)
        {
            m_mediaplayer->setPosition(0);
        }
        else if (keyPress->key() == Qt::Key_N)
        {
            renameVideo();
        }
        else
        {
            keyPress->ignore();
            return false;
        }

        event->accept();
        return true;
    }
    else
    {
        event->ignore();
        return false;
    }

    return QVideoWidget::eventFilter(obj, event);
}

void Movieplayer::resizeEvent(QResizeEvent *event)
{
    // keep a 16 : 9 ratio at all times

    int height = event->size().height();
    int width = event->size().width();

    if (height > event->oldSize().height())
    {
        width = ((height / 9) * 16);
    }
    if (width > event->oldSize().width())
    {
        height  = ((width / 16) * 9);
    }

    this->setMinimumSize(width, height);

    QVideoWidget::resizeEvent(event);
}

void Movieplayer::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit resizeWindow(false);

    if (!itmes->isVisible())
        itmes->show();

    if (!controls->isVisible())
        controls->show();

    QVideoWidget::mouseDoubleClickEvent(event);
}

void Movieplayer::mouseMoveEvent(QMouseEvent *event)
{
    this->setToolTip(getCurrentFilename(playList->currentIndex()));
    QVideoWidget::mouseMoveEvent(event);
}

void Movieplayer::resumeVideo(int index, bool first)
{
    if (index == -1)
        return;

    QSettings settings(COMPANY, APPNAME);
    //settings.clear();

    if (first)
    {
        QString val = settings.value("lastPlayed").toString();

        if (!val.isEmpty())
        {
            index = getCurrentIndex(val);
            //qDebug() << QString("Resuming %1 and Index %2").arg(val, QString::number(index));
        }
    }

    QString oldkey = getCurrentFilename(playList->currentIndex());
    QString newKey = getCurrentFilename(index);

    if (!first)
    {
        settings.setValue(oldkey, m_mediaplayer->position());
        settings.setValue("lastPlayed", oldkey);
        //qDebug() << QString("Saving key for %1 and position %2").arg(oldkey, QString::number(int(pos)));
    }

    playList->setCurrentIndex(index);
    m_mediaplayer->play();

    if (settings.contains(newKey))
    {
        m_mediaplayer->setPosition(settings.value(newKey).toLongLong());
    }
}

int Movieplayer::getCurrentIndex(const QString &name)
{
    QMap<int, QString>::const_iterator i = vidMap.constBegin();

    while (i != vidMap.constEnd())
    {
        if (i.value() == name)
        {
            return int(i.key());
        }

        //qDebug() << QString("getCurrentIndex: Name %1 : Index %2").arg(i.value(), QString::number(i.key()));
        ++i;
    }

    return 0;
}

QString Movieplayer::getCurrentFilename(int index)
{
    return vidMap[index];
}

