#include "movieplayer.h"
#include "ui_movieplayer.h"
#include "text.h"

Movieplayer::Movieplayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Movieplayer()),
    itmes(new PlayListItems())
{
    ui->setupUi(this);

    m_mediaplayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    videoWidget = new QVideoWidget(this);
    playList = new QMediaPlaylist(m_mediaplayer);

    m_mediaplayer->setVideoOutput(videoWidget);
    ui->videoLayout->addWidget(videoWidget);

    connect(this, SIGNAL(addPlayList(QString)), itmes, SLOT(addPlayListItems(QString)));
    //connect(this, SIGNAL(updatePlaylist(int)), itmes, SLOT(updateList(int)));
    //connect(itmes, SIGNAL(closeAllWindows()), this, SIGNAL(closeApp()));
    connect(this, SIGNAL(closeApp()), itmes, SLOT(close()));
    connect(itmes, SIGNAL(newItemSelected(QListWidgetItem*)), this, SLOT(playSelectedItem(QListWidgetItem*)));
    connect(this, SIGNAL(removeListItem()), itmes, SLOT(removeListItem()));
    connect(itmes, SIGNAL(changeVolume(int)), this, SLOT(setVideoVolume(int)));
    connect(m_mediaplayer, SIGNAL(positionChanged(qint64)), this, SLOT(getVideoDuration(qint64)));
    connect(this, SIGNAL(displayVideoDuration(qint64,qint64)), itmes, SLOT(displayPlayTime(qint64,qint64)));
    connect(playList, SIGNAL(currentIndexChanged(int)), itmes, SLOT(updateList(int)));
}

Movieplayer::~Movieplayer()
{
    delete ui;

    if(itmes)
        delete itmes;

    if(settings != nullptr) {
        settings->clear();
        delete settings;
    }
}

void Movieplayer::loadMediaPlaylist(const QString &mediaPath)
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

    if(foundVid)
    {
        itmes->show();
        playList->setPlaybackMode(QMediaPlaylist::Loop);
        m_mediaplayer->setPlaylist(playList);
        m_mediaplayer->play();
        m_mediaplayer->setVolume(itmes->lastSavedVolume());
    }
}

void Movieplayer::isMediaAvailable(bool found)
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

void Movieplayer::playSelectedItem(QListWidgetItem *item)
{
    Q_UNUSED(item);
    int index = itmes->getSelectedItem();

    bool found = resumeVideo(index);

    if(!found) {

        playList->setCurrentIndex(index);
    }
    m_mediaplayer->play();
}

void Movieplayer::setVideoVolume(int vol)
{
    m_mediaplayer->setVolume(vol);
    m_mediaplayer->setMuted(false);
}

void Movieplayer::getVideoDuration(qint64 length)
{
    qint64 num = m_mediaplayer->duration();
    emit displayVideoDuration(length, num);
}

void Movieplayer::removeCurrentVideo()
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
        playList->clear();
        itmes->resetPlaylist();
        loadMediaPlaylist(dirName);
    }

    if(playList->isEmpty())
        m_mediaplayer->stop();
}

bool Movieplayer::eventFilter(QObject *obj, QEvent* event)
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
            m_mediaplayer->setPosition(m_mediaplayer->position() +  seekPostion);
        }
        else if(keyPress->key() == Qt::Key_Left)
        {
            m_mediaplayer->setPosition(m_mediaplayer->position() - seekPostion);
        }
        else if (keyPress->key() == Qt::Key_Space) {

            if (m_mediaplayer->state() == QMediaPlayer::PausedState)
            {
                qint64 pos = m_mediaplayer->position();
                m_mediaplayer->play();
                playList->setCurrentIndex(playList->currentIndex());
                m_mediaplayer->setPosition(pos);
            }
            else {
                m_mediaplayer->pause();
            }
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
            (!m_mediaplayer->isMuted())? m_mediaplayer->setMuted(true) : m_mediaplayer->setMuted(false);
        }
        else if (keyPress->key() == Qt::Key_X)
        {
            emit closeApp();
        }
        else if (keyPress->key() == Qt::Key_Escape || keyPress->key() == Qt::Key_F)
        {
            emit resizeWindow(true);
        }
        else if (keyPress->key() == Qt::Key_D)
        {
            removeCurrentVideo();
        }
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
            m_mediaplayer->setPosition(0);
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

bool Movieplayer::resumeVideo(const int &index)
{
    QString company = "NunyaBiz";
    QString appName = "MovieWidget";

    if (!settings)
        settings = new QSettings(company, appName);

    qint64 pos = m_mediaplayer->position();
    QString oldkey = "Key_" + QString::number(playList->currentIndex());
    QString newKey = "Key_" + QString::number(index);

    settings->setValue(oldkey, int(pos));
    qDebug() << QString("Saving key for %1 and position %2").arg(oldkey, QString::number(int(pos)));

    if(settings->contains(newKey))
    {
        playList->setCurrentIndex(index);
        m_mediaplayer->setPosition(qint64(settings->value(newKey).toInt()));
        return true;
    }

    return false;
}

/*void Movieplayer::resizeEvent(QResizeEvent *event)
{
    //videoWidget->restoreGeometry(i)
    qDebug()  << "Old size: " + QString::number(videoWidget->size().height()) + ":" + QString::number(videoWidget->size().width());
    if (event->size() != event->oldSize())
    {
        qDebug()  << "New size : " + QString::number(videoWidget->size().height()) + ":" + QString::number(videoWidget->size().width());
    }
}*/
