#include "movieplayer.h"
#include "ui_movieplayer.h"
#include "text.h"

#include <QInputDialog>
#include <QMediaObject>

Movieplayer::Movieplayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Movieplayer()),
    itmes(new PlayListItems())
{
    ui->setupUi(this);

    m_mediaplayer = new QMediaPlayer(0, QMediaPlayer::VideoSurface);
    videoWidget = new QVideoWidget();
    playList = new QMediaPlaylist(m_mediaplayer);

    m_mediaplayer->setVideoOutput(videoWidget);
    ui->videoLayout->addWidget(videoWidget, Qt::AlignCenter);
    videoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);

    connect(this, SIGNAL(addPlayList(QString)), itmes, SLOT(addPlayListItems(QString)));
    connect(this, SIGNAL(closeApp()), itmes, SLOT(close()));
    connect(itmes, SIGNAL(newItemSelected(QListWidgetItem*)), this, SLOT(playSelectedItem(QListWidgetItem*)));
    connect(itmes, SIGNAL(changeVolume(int)), this, SLOT(setVideoVolume(int)));
    //connect(itmes, SIGNAL(videoSliderMoved(int)), this, SLOT(videoSliderMoved(int)));
    connect(m_mediaplayer, SIGNAL(positionChanged(qint64)), this, SLOT(getVideoDuration(qint64)));
    //connect(this, SIGNAL(displayVideoDuration(qint64,qint64)), itmes, SLOT(displayPlayTime(qint64,qint64)));
    connect(playList, SIGNAL(currentIndexChanged(int)), itmes, SLOT(updateList(int)));
    connect(itmes, SIGNAL(playPauseVideo()), this, SLOT(playPause()));
    connect(itmes, SIGNAL(gotoNextVideo()), this, SLOT(gotoNext()));
    connect(itmes, SIGNAL(gotoPreviousVideo()), this, SLOT(gotoPrevious()));
}

Movieplayer::~Movieplayer()
{
    if(playList)
        resumeVideo(playList->currentIndex());

    delete ui;

    delete itmes;
}

void Movieplayer::loadMediaPlaylist(const QString &mediaPath)
{
    QString checkVid = "ls " + QString(DOWNLOADS)  + " | grep -q '.mp4' > /dev/null";
    QString getVideo = "mv "+ QString(DOWNLOADS) + "*.mp4 " +  QString(VIDEO_DIR);
    int count = 0;

    itmes->show();
    dirName = mediaPath;

    if(system(checkVid.toLocal8Bit().data()) == 0)
        system(getVideo.toLocal8Bit().data());

    QStringList filters;
    filters << "*.mp4" << "*.mov";

    videoList = QDir(mediaPath).entryInfoList(filters, QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo entry : videoList)
    {
        playList->addMedia(QUrl::fromLocalFile(mediaPath + entry.fileName()));
        QString vidName = "(" + QString::number(count) + ") " + entry.fileName();
        emit addPlayList(vidName);
        count++;
    }

    isMediaAvailable((videoList.size() != 0));

    if(!videoList.isEmpty())
    {
        itmes->show();
        playList->setPlaybackMode(QMediaPlaylist::Loop);
        m_mediaplayer->setPlaylist(playList);
        m_mediaplayer->setVolume(itmes->lastSavedVolume());
    }

    resumeVideo(0, true);
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

    resumeVideo(index);

    // Update player title
    //emit updateWindowTitle(videoList.at(index).fileName());
}

void Movieplayer::setVideoVolume(int vol)
{
    m_mediaplayer->setVolume(vol);
    if (m_mediaplayer->isMuted())
        m_mediaplayer->setMuted(false);
}

void Movieplayer::getVideoDuration(qint64 length)
{
    if(!itmes) return;

    qint64 num = m_mediaplayer->duration();
    //emit displayVideoDuration(length, num);

    int timeRemaining = num - length;
    QString remaining = itmes->getVideoTime(timeRemaining);

    // Update player title
    emit updateWindowTitle(itmes->getItemName() + "\t\t" + remaining);
}

/*void Movieplayer::videoSliderMoved(int value)
{
    m_mediaplayer->setPosition(value);
}*/

void Movieplayer::removeCurrentVideo()
{
    QString videoName = videoList.at(itmes->getSelectedItem()).fileName();
    QFile removeVid(dirName + "/" + videoName);

    if(!removeVid.exists())  return;

    int exec = QMessageBox::question(this, QString("Delete Video?"),
                                     QString("Are you sure you want to delete\n%1"
                                             "\nClick yes to confirm.").arg(videoName),
                                     QMessageBox::Yes | QMessageBox::No);

    if(exec == QMessageBox::No) return;

    if(removeVid.remove()) {
        // if deletion was successful
        reloadContent();
    }
}

void Movieplayer::renameVideo()
{
    QString name = videoList.at(playList->currentIndex()).fileName();

    bool ok = false;

    QString text = QInputDialog::getText(  this,  tr("Rename video"),
               tr("Enter new name"),
               QLineEdit::Normal, "", &ok );

    if (ok && !text.isEmpty() )
    {
        if (!text.endsWith(".mp4"))
            text.append(".mp4");

        if (QFile::rename(dirName + "/" + name, dirName + "/" + text)) {
            reloadContent();
        }
    }
    else {
        renameVideo();
    }
}

void Movieplayer::reloadContent()
{
    playList->clear();
    itmes->resetPlaylist();
    loadMediaPlaylist(dirName);
    resumeVideo(0);
}

void Movieplayer::playPause()
{
    if (m_mediaplayer->state() == QMediaPlayer::PausedState) {
        m_mediaplayer->play();
        resumeVideo(playList->currentIndex());
    }
    else {
        m_mediaplayer->pause();
    }
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

        if(keyPress->key() == Qt::Key_Right)
        {
            m_mediaplayer->setPosition(m_mediaplayer->position() +  seekPostion);
        }
        else if(keyPress->key() == Qt::Key_Left)
        {
            m_mediaplayer->setPosition(m_mediaplayer->position() - seekPostion);
        }
        else if (keyPress->key() == Qt::Key_Space) {

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

    return QWidget::eventFilter(obj, event);
}

void Movieplayer::resizeEvent(QResizeEvent *event)
{
    // keep a 16 : 9 ratio at all times

    int height = event->size().height();
    int width = event->size().width();

    //qDebug() << QString("Width: %1 and Height: %2").arg(QString::number(ui->graphicsView->size().width()), QString::number(ui->graphicsView->size().height()));
    //qDebug() << QString("This Width: %1 and This Height: %2").arg(QString::number(this->size().width()), QString::number(this->size().height()));

    if (height > event->oldSize().height()) {
        width = ((height / 9) * 16);
        this->setMinimumSize(width, height);
    }
    else if (width > event->oldSize().width()) {
        height  = ((width / 16) * 9);
        this->setMinimumSize(width, height);

    } else {
        // fix me
    }

    QWidget::resizeEvent(event);
}

void Movieplayer::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit resizeWindow(false);
    if (!itmes->isVisible())
        itmes->show();

    QWidget::mouseDoubleClickEvent(event);
}

void Movieplayer::resumeVideo(int index, bool first)
{
    if(index == -1) return;

    QSettings settings("NunyaBiz", "iPlay Movie");
    //settings.clear();
    if (first) {
        if (settings.contains("lastPlayed")) {
            index = settings.value("lastPlayed").toInt();
            resumeVideo(index);
        }
    }

    qint64 pos = m_mediaplayer->position();
    QString oldkey = "Key_" + videoList.at(playList->currentIndex()).fileName();
    QString newKey = "Key_" + videoList.at(index).fileName();

    if(!first) {
        settings.setValue(oldkey, m_mediaplayer->position());
        settings.setValue("lastPlayed", playList->currentIndex());
        qDebug() << QString("Saving key for %1 and position %2").arg(oldkey, QString::number(int(pos)));
    }

    playList->setCurrentIndex(index);
    m_mediaplayer->play();

    if(settings.contains(newKey))
    {
        m_mediaplayer->setPosition(settings.value(newKey).toLongLong());
    }
}


