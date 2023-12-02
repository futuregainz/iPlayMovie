#include "movieplayer.h"

#include <QInputDialog>
//#include <QMediaObject>

Movieplayer::Movieplayer(QVideoWidget *parent) :
    QVideoWidget(parent)
{
    controls = new VideoControls();
    playList = new PlayListItems();
    m_mediaplayer = new QMediaPlayer();
    audioOutput = new QAudioOutput();
    m_mediaplayer->setAudioOutput(audioOutput);

    m_mediaplayer->setVideoOutput(this);
    this->setAspectRatioMode(Qt::KeepAspectRatio);

    connect(this, SIGNAL(addPlayList(QString)), playList, SLOT(addPlayListItem(QString)));
    connect(this, SIGNAL(closeApp()), playList, SLOT(close()));
    connect(this, SIGNAL(closeApp()), controls, SLOT(close()));
    connect(playList, SIGNAL(newItemSelected(QListWidgetItem*)), this, SLOT(playSelectedItem(QListWidgetItem*)));
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
    resumeVideo(false);
    delete playList;
    delete controls;
}

void Movieplayer::loadMediaPlaylist(const QString &mediaPath)
{
    QString homePaht = QDir::homePath();
    dirName = (mediaPath.endsWith("/"))? mediaPath : mediaPath + "/";
    playList->playlistClear();

    QStringList filters;
    filters << "*.mp4" << "*.mov";

    videoList = QDir(homePaht + QString(DOWNLOADS)).entryInfoList(filters, QDir::AllEntries | QDir::NoDotAndDotDot);

    if (videoList.size() > 0)
    {
        foreach (QFileInfo entry, videoList)
        {
            QString tmpVid = dirName + entry.fileName();

            if (videoNameTaken(tmpVid))
            {
                int count = 0;
                // Keep trying to rename video to a new name.
                while (videoNameTaken(tmpVid))
                {
                    tmpVid = tmpVid.replace(".mp4", "_");
                    tmpVid = tmpVid + QString::number(count) + ".mp4";
                    count ++;
                }
            }

            QFile::rename(entry.absoluteFilePath(), tmpVid);
        }
    }

    videoList = QDir(dirName).entryInfoList(filters, QDir::AllEntries | QDir::NoDotAndDotDot);

    isMediaAvailable((videoList.size() > 0));

    foreach (QFileInfo entry, videoList)
    {
        QString vidname = entry.fileName();

        if (vidname.contains(" "))
        {
            vidname = vidname.replace(" ", "-");
            QFile::rename(dirName + entry.fileName(), dirName + vidname);
        }

        playList->addMedia(dirName + vidname);
        emit addPlayList(vidname);
    }

    m_mediaplayer->setLoops(QMediaPlayer::Infinite);
    setVideoVolume(playList->lastSavedVolume());
    resumeVideo(true);
    playList->show();
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
    playVideo(playList->currentIndex());
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
    audioOutput->setMuted(!audioOutput->isMuted());
}

void Movieplayer::removeCurrentVideo()
{
    QString videoName = playList->currentMedia();
    QFile removeVid(videoName);

    if (!removeVid.exists())
        return;

    if (QMessageBox::question(this, QString("Delete Video?"), QString("Are you sure you want to delete\n%1").arg(videoName),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) return;

    if (removeVid.remove())
    {
        //int index = playList->currentIndex();
        loadMediaPlaylist(dirName);
        //playList->removeItem(index);
        //playList->removeMedia(index);
        //playList->setCurrentIndex();
    }
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

        QString newName = dirName + text;

        if (videoNameTaken(newName))
        {
            QMessageBox::information(this, "", "A video with that name already exists.\n", QMessageBox::Ok);
            renameVideo();
        }

        if (QFile::rename(name, newName))
        {
            loadMediaPlaylist(dirName);
            //int index = playList->currentIndex();
            //playList->renameItem(index, text);
            //playList->renameMedia(index, newName);
        }
    }
    else
    {
        renameVideo();
    }
}

void Movieplayer::playPause()
{
    bool isPaused = (m_mediaplayer->playbackState() == QMediaPlayer::PausedState);

     if (isPaused)
    {
        m_mediaplayer->play();
    }
    else
    {
        m_mediaplayer->pause();
    }

    emit videoPlaying(isPaused);
}

void Movieplayer::gotoPrevious()
{
    playVideo(playList->previousIndex());
}

void Movieplayer::gotoNext()
{
    playVideo(playList->nextIndex());
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
            int vol = audioOutput->volume() + 1;
            setVideoVolume(vol);
            emit volumeChangedViaShortcuts(vol);
        }
        else if (keyPress->key() == Qt::Key_Y)
        {
            int vol = audioOutput->volume() - 1;
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
            playList->hide();
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

    if (!playList->isVisible())
        playList->show();

    if (!controls->isVisible())
        controls->show();

    QVideoWidget::mouseDoubleClickEvent(event);
}

void Movieplayer::mouseMoveEvent(QMouseEvent *event)
{
    this->setToolTip(playList->currentMedia());
    QVideoWidget::mouseMoveEvent(event);
}

bool Movieplayer::videoNameTaken(const QString &vidName)
{
    return QFile::exists(vidName);
}

void Movieplayer::resumeVideo(bool first)
{
    QSettings settings(COMPANY, APPNAME);
    //settings.clear();
    qint64 pos = 0;

    if (first)
    {
        QString curr = settings.value("lastPlayed").toString();

        if (curr.isEmpty())
        {
            curr = playList->currentMedia();
        }

        m_mediaplayer->setSource(QUrl::fromLocalFile(curr));
        m_mediaplayer->play();

        if (settings.contains(curr))
        {
            pos = settings.value(curr).toLongLong();
            m_mediaplayer->setPosition(pos);
        }
    }
    else
    {
        pos = m_mediaplayer->position();
        settings.setValue(playList->currentMedia(), pos);
        settings.setValue("lastPlayed", playList->currentMedia());
    }
}

void Movieplayer::playVideo(int index)
{
    playList->setCurrentIndex(index);
    QString video = playList->currentMedia();
    m_mediaplayer->setSource(QUrl::fromLocalFile(video));
    m_mediaplayer->play();
}
