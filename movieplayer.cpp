#include "movieplayer.h"
#include "ui_movieplayer.h"
#include "text.h"

MoviePlayer::MoviePlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoviePlayer()),
    itmes(new PlayListItems())
{
    ui->setupUi(this);

    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    videoWidget = new QVideoWidget(this);
    playList = new QMediaPlaylist(m_mediaPlayer);

    m_mediaPlayer->setVideoOutput(videoWidget);
    ui->videoLayout->addWidget(videoWidget);

    connect(this, SIGNAL(addPlayList(QString)), itmes, SLOT(addPlayListItems(QString)));
    //connect(this, SIGNAL(updatePlaylist(int)), itmes, SLOT(updateList(int)));
    //connect(itmes, SIGNAL(closeAllWindows()), this, SIGNAL(closeApp()));
    connect(this, SIGNAL(closeApp()), itmes, SLOT(close()));
    connect(itmes, SIGNAL(newItemSelected(QListWidgetItem*)), this, SLOT(playSelectedItem(QListWidgetItem*)));
    connect(this, SIGNAL(removeListItem()), itmes, SLOT(removeListItem()));
    connect(itmes, SIGNAL(changeVolume(int)), this, SLOT(setVideoVolume(int)));
    connect(m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(getVideoDuration(qint64)));
    connect(this, SIGNAL(displayVideoDuration(qint64,qint64)), itmes, SLOT(displayPlayTime(qint64,qint64)));
    connect(playList, SIGNAL(currentIndexChanged(int)), itmes, SLOT(updateList(int)));
}

MoviePlayer::~MoviePlayer()
{
    delete ui;

    if(itmes)
        delete itmes;

    if(settings != nullptr) {
        settings->clear();
        delete settings;
    }
}

void MoviePlayer::loadMediaPlaylist(const QString &mediaPath)
{
    QString checkVid = "ls " + QString(DOWNLOADS)  + " | grep -q '.mp4' > /dev/null";
    QString getVideo = "mv "+ QString(DOWNLOADS) + "*.mp4 " +  QString(VIDEO_DIR);

    //qDebug() << checkVid + " and " + getVideo;

    itmes->show();
    bool foundVid = false;
    dirName = mediaPath;

    if(system(checkVid.toLocal8Bit().data()) == 0)
        system(getVideo.toLocal8Bit().data());

    QStringList filters;
    filters << "*.mp4" << "*.mov";

    QFileInfoList videoList = QDir(mediaPath).entryInfoList(filters, QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo entry : videoList)
    {
        playList->addMedia(QUrl::fromLocalFile((mediaPath) + entry.fileName()));
        emit addPlayList(entry.fileName());
        foundVid = true;
    }

    isMediaAvailable(foundVid);

    playList->setPlaybackMode(QMediaPlaylist::Loop);
    m_mediaPlayer->setPlaylist(playList);
    m_mediaPlayer->play();
}

void MoviePlayer::isMediaAvailable(bool found)
{
    if(found) return;

    int exec = QMessageBox::critical(this, QString("Empty PlayList"),
                                     QString("No valid media (.mp4) was found in '%1'\n Try a different folder?").arg(dirName),
                                     QMessageBox::Yes | QMessageBox::No);

    if (exec == QMessageBox::Yes)
    {
        dirName = QFileDialog::getExistingDirectory(this, tr("Select a Different Folder"),
                                                    dirName,
                                                    QFileDialog::ShowDirsOnly);

        (!dirName.isEmpty()) ? loadMediaPlaylist(dirName + "/") :  isMediaAvailable(false);
    }
    else
    {
        emit closeApp();
    }
}

void MoviePlayer::playSelectedItem(QListWidgetItem *item)
{
    Q_UNUSED(item);
    int index = itmes->getSelectedItem();

    bool found = resumeVideo(index);

    if(!found) {

        playList->setCurrentIndex(index);
    }
    m_mediaPlayer->play();
}

void MoviePlayer::setVideoVolume(int vol)
{
    m_mediaPlayer->setVolume(vol);
    m_mediaPlayer->setMuted(false);
}

void MoviePlayer::getVideoDuration(qint64 length)
{
    qint64 num = m_mediaPlayer->duration();
    emit displayVideoDuration(length, num);
}

/*void MoviePlayer::removeCurrentVideo()
{
    QString videoName = itmes->getSelectedItemName();
    QFile removeVid(dirName + "/" + videoName);

    if(!removeVid.exists())  return;

    int exec = QMessageBox::question(this, QString("Delete Video?"),
                                     QString("Are you sure you want to delete\n%1"
                                             "\nClick yes to confirm.").arg(videoName),
                                     QMessageBox::Yes | QMessageBox::No);

    if(exec == QMessageBox::No) return;

    if(removeVid.remove())
    { // if deletion was successful
        playList->removeMedia(itmes->getSelectedItem());
        playList->next();
        QListWidgetItem item;
        playSelectedItem(&item);
        emit removeListItem();
    }

    if(playList->isEmpty())
        m_mediaPlayer->stop();
}*/

bool MoviePlayer::eventFilter(QObject *obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        emit resizeWindow(false);
        if (!itmes->isVisible())
            itmes->show();
    }
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyPress = static_cast<QKeyEvent *>(event);

        if(keyPress->key() == Qt::Key_Right)
        {
            m_mediaPlayer->setPosition(m_mediaPlayer->position() +  seekPostion);
        }
        else if(keyPress->key() == Qt::Key_Left)
        {
            m_mediaPlayer->setPosition(m_mediaPlayer->position() - seekPostion);
        }
        else if (keyPress->key() == Qt::Key_Space) {
            if (m_mediaPlayer->state() == QMediaPlayer::PausedState) {
                qint64 pos = m_mediaPlayer->position();
                m_mediaPlayer->play();
                playList->setCurrentIndex(playList->currentIndex());
                m_mediaPlayer->setPosition(pos);
            }
            else {m_mediaPlayer->pause(); }
        }
        else if (keyPress->key() == Qt::Key_Up)
        {
            bool found = resumeVideo(playList->previousIndex());

            if(!found) {

                playList->previous();
            }
        }
        else if (keyPress->key() == Qt::Key_Down)
        {
            bool found = resumeVideo(playList->nextIndex());

            if(!found) {

                playList->next();
            }
        }
        else if (keyPress->key() == Qt::Key_M)
        {
            (!m_mediaPlayer->isMuted())? m_mediaPlayer->setMuted(true) : m_mediaPlayer->setMuted(false);
        }
        else if (keyPress->key() == Qt::Key_X)
        {
            emit closeApp();
        }
        else if (keyPress->key() == Qt::Key_Escape || keyPress->key() == Qt::Key_F)
        {
            emit resizeWindow(true);
        }
        /*else if (keyPress->key() == Qt::Key_D)
        {
            removeCurrentVideo();
        }*/
        else if (keyPress->key() == Qt::Key_L )
        {
            if (playList->playbackMode() == QMediaPlaylist::Loop)
            {
                playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            }
            else
            {
                playList->setPlaybackMode(QMediaPlaylist::Loop);
            }
        }
        else if (keyPress->key() == Qt::Key_R)
        {
            m_mediaPlayer->setPosition(0);
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

    return QWidget::eventFilter(obj, event);
}

bool MoviePlayer::resumeVideo(const int &index)
{
    QString company = "NunyaBiz";
    QString appName = "MovieWidget";

    if (!settings)
        settings = new QSettings(company, appName);

    qint64 pos = m_mediaPlayer->position();
    QString oldkey = "Key_" + QString::number(playList->currentIndex());
    QString newKey = "Key_" + QString::number(index);

    settings->setValue(oldkey, int(pos));
    qDebug() << QString("Saving key for %1 and position %2").arg(oldkey, QString::number(int(pos)));

    if(settings->contains(newKey))
    {
        playList->setCurrentIndex(index);
        m_mediaPlayer->setPosition(qint64(settings->value(newKey).toInt()));
        return true;
    }

    return false;
}

/*void MoviePlayer::resizeEvent(QResizeEvent *event)
{
    //videoWidget->restoreGeometry(i)
    qDebug()  << "Old size: " + QString::number(videoWidget->size().height()) + ":" + QString::number(videoWidget->size().width());
    if (event->size() != event->oldSize())
    {
        qDebug()  << "New size : " + QString::number(videoWidget->size().height()) + ":" + QString::number(videoWidget->size().width());
    }
}*/
