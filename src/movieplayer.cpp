#include "movieplayer.h"

#include <QInputDialog>
//#include <QMediaObject>

Movieplayer::Movieplayer(QVideoWidget *parent) :
    QVideoWidget(parent),
    itmes(new PlayListItems())
{
    controls = new VideoControls();

    m_mediaplayer = new QMediaPlayer();
    playList = new QMediaPlaylist();
    audioOutput = new QAudioOutput();
    m_mediaplayer->setAudioOutput(audioOutput);

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
    connect(audioOutput, SIGNAL(mutedChanged(bool)), controls, SLOT(videoMuted(bool)));
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

    //int count = 0;

    foreach (QFileInfo entry, videoList)
    {
        QString vidname = entry.fileName();

        if (vidname.contains(" "))
        {
            vidname = vidname.replace(" ", "-");
            QFile::rename(mediaPath + entry.fileName(), mediaPath + vidname);
        }

        playList->addMedia(mediaPath + vidname);
        //playList->setCurrentIndex(count);
        //vidMap[count] = vidname;
        emit addPlayList(vidname);
        //count++;
    }

    //qDebug() << QString("Count:  %1").arg(QString::number(count));

    //playList->setPlaybackMode(QMediaPlaylist::Loop);
    m_mediaplayer->setLoops(QMediaPlayer::Infinite);
    //m_mediaplayer->setPlaylist(playList);
    //m_mediaplayer->setSource(playList->currentMedia());
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
    playList->setCurrentIndex(dirName + item->text());
    resumeVideo(playList->currentIndex());
}

void Movieplayer::setVideoVolume(int vol)
{
    if (vol < 0)
        return;

    audioOutput->setVolume(vol);

    if (audioOutput->isMuted())
        audioOutput->setMuted(false);
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
    (!audioOutput->isMuted())? audioOutput->setMuted(true) : audioOutput->setMuted(false);
}

void Movieplayer::removeCurrentVideo()
{
    QString videoName = playList->currentMedia();
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
    QString name = playList->currentMedia();
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
    ..itmes->resetPlaylist();
    //vidMap.clear();
    loadMediaPlaylist(QDir::homePath() + QString(VIDEO_DIR));
}

void Movieplayer::playPause()
{
    bool isPaused = m_mediaplayer->isPlaying();

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
            int vol = audioOutput->volume() + 10;
            setVideoVolume(vol);
            emit volumeChangedViaShortcuts(vol);
        }
        else if (keyPress->key() == Qt::Key_Y)
        {
            int vol = audioOutput->volume() - 10;
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
            if (m_mediaplayer->loops() == QMediaPlayer::Infinite)
            {
                m_mediaplayer->setLoops(QMediaPlayer::Once);
            }
            else
            {
                m_mediaplayer->setLoops(QMediaPlayer::Infinite);
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
    this->setToolTip(playList->currentMedia());
    QVideoWidget::mouseMoveEvent(event);
}

void Movieplayer::resumeVideo(int index, bool first)
{
    if (index == -1)
        return;

    QSettings settings(COMPANY, APPNAME);
    //settings.clear();
    QString oldKey = playList->currentMedia();
    QString newKey = playList->getVideoName(index);

    if (first)
    {
        QString val = settings.value("lastPlayed").toString();

        if (!val.isEmpty())
        {
            newKey = val;
            //index = getCurrentIndex(val);
        }
    }
    else
    {
        playList->setCurrentIndex(index);
        newKey = playList->currentMedia();
        settings.setValue(oldKey, m_mediaplayer->position());
        settings.setValue("lastPlayed", oldKey);
        //qDebug() << QString("Saving key for %1 and position %2").arg(oldkey, QString::number(int(pos)));
    }

    m_mediaplayer->setSource(QUrl::fromLocalFile(newKey));
    m_mediaplayer->play();

    if (settings.contains(newKey))
    {
        m_mediaplayer->setPosition(settings.value(newKey).toLongLong());
    }
}
