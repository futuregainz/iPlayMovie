#include "movieplayer.h"
#include "ui_movieplayer.h"
#include "text.h"

MoviePlayer::MoviePlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoviePlayer),
    itmes(new PlayListItems)
{
    ui->setupUi(this);

    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    videoWidget = new QVideoWidget(this);
    playList = new QMediaPlaylist(m_mediaPlayer);

    m_mediaPlayer->setVideoOutput(videoWidget);
    ui->videoLayout->addWidget(videoWidget);

    connect(this, SIGNAL(addPlayList(QString)), itmes, SLOT(addPlayListItems(QString)));
    connect(itmes, SIGNAL(closeAllWindows()), this, SIGNAL(closeApp()));
    connect(this, SIGNAL(closeApp()), itmes, SLOT(close()));
    connect(itmes, SIGNAL(newItemSelected(QListWidgetItem*)), this, SLOT(playSelectedItem(QListWidgetItem*)));
}

MoviePlayer::~MoviePlayer()
{
    delete ui;

    if(itmes) delete itmes;
}

void MoviePlayer::loadMediaPlaylist(const QString &mediaPath)
{
    itmes->show();
    bool foundVid = false;

    if(system("ls /Users/moegainz/Downloads/ | grep -q '.mp4' > /dev/null") == 0)
        system("mv /Users/moegainz/Downloads/*.mp4 /Users/moegainz/.metadata/");


    QFileInfoList videoList = QDir((mediaPath)).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(QFileInfo entry : videoList)
    {
        if(!entry.fileName().endsWith(".mp4")) continue;

        playList->addMedia(QUrl::fromLocalFile((mediaPath) + entry.fileName()));
        emit addPlayList(entry.fileName());
        foundVid = true;
    }

    isMediaAvailable(foundVid);

    playList->setPlaybackMode(QMediaPlaylist::Loop);

    playList->setCurrentIndex(0);
    m_mediaPlayer->setPlaylist(playList);
    m_mediaPlayer->play();
}

void MoviePlayer::isMediaAvailable(bool found)
{
    if(found) return;

    int exec = QMessageBox::critical(nullptr, QObject::tr("Empty PlayList"),
                                     QObject::tr("No valid media (.mp4) was found in 'Movies'\n Try a different folder?"),
                                     QMessageBox::Yes | QMessageBox::No);
    if (exec == QMessageBox::Yes)
    {
        QString dirName = QFileDialog::getExistingDirectory(this, tr("Select a Different Folder"),
                                                     "/Users/moegainz/Movies",
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!dirName.isEmpty()) {
            loadMediaPlaylist(dirName + "/");
        }
        else {
            isMediaAvailable(false);
        }
    }
    else {

        emit closeApp();
    }
}

void MoviePlayer::playSelectedItem(QListWidgetItem *item)
{
    Q_UNUSED(item);
    int index = itmes->getSelectedItem();
    playList->setCurrentIndex(index);
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
        else if (keyPress->key() == Qt::Key_P) {

            emit resizeWindow(false);
            if (!itmes->isVisible()) { itmes->showNormal(); }
            else { itmes->hide(); }
        }
        else if (keyPress->key() == Qt::Key_Escape || keyPress->key() == Qt::Key_F) {

            emit resizeWindow(true);
        }
        /*else if (keyPress->key() == Qt::Key_D) {

            removeCurrentVideo(playList->currentIndex());
        }*/
        else {

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
