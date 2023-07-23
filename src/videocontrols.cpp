#include "videocontrols.h"
#include "ui_videocontrols.h"
#include <QSettings>
#include <QStyle>
#include <QTime>
#include "text.h"


VideoControls::VideoControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoControls)
{
    ui->setupUi(this);

    QSettings settings(COMPANY, APPNAME);
    this->restoreGeometry(settings.value("controls_geometry").toByteArray());
    lastSavedVol = settings.value("videoVolume").toInt();
    ui->volCotrl->setValue(lastSavedVol);

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->backButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    //ui-> ->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->muteVidButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    //this->setStyleSheet("background-color: transparent;");
}

VideoControls::~VideoControls()
{
    delete ui;
}

void VideoControls::on_backButton_clicked()
{
    emit gotoPreviousVideo();
}

void VideoControls::on_playButton_clicked()
{
    emit playPauseVideo();
    (isVidPaused)? ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause)) :
                   ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

void VideoControls::on_nextButton_clicked()
{
    emit gotoNextVideo();
}

void VideoControls::displayVideoDuration(qint64 length, qint64 num)
{
    ui->timePassed->setText(getVideoTime(length));
    ui->timeLeft->setText(getVideoTime(num - length));
    ui->videoProgress->update();
    ui->videoProgress->setValue(length);
}

void VideoControls::setVideoRange(qint64 durantion)
{
    ui->videoProgress->setRange(0, durantion);
}

void VideoControls::videoPaused(bool isPaused)
{
    isVidPaused = isPaused;
}

void VideoControls::videoMuted(bool isMuted)
{
    (isMuted)? ui->muteVidButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted)):
               ui->muteVidButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
}

void VideoControls::volumeChanged(int vol)
{
    ui->volCotrl->setValue(vol);
}

QString VideoControls::getVideoTime(const int &miliseconds)
{
    QString writeTime;
    QTime time(0,0,0,0);
    time = time.addMSecs(miliseconds);

    if (miliseconds < secToMiliSecs) // if less than 60 seconds
    {
        writeTime = "00:00:" + prependZero(time.second());
    }
    else if (miliseconds < minToMiliSecs)
    {
        writeTime = "00:" + prependZero(time.minute()) + ":" + prependZero(time.second());
    }
    else  // video is longer than one hour
    {
        writeTime = prependZero(time.hour()) + ":" + prependZero(time.minute()) + ":" + prependZero(time.second());
    }

    return writeTime;
}

QString VideoControls::prependZero(int val)
{
    return (val < 10)? "0" + QString::number(val) : QString::number(val);
}

void VideoControls::on_videoProgress_sliderMoved(int position)
{
    emit videoSliderMoved(position);
}

void VideoControls::closeEvent(QCloseEvent *bar)
{
    this->close();

    QSettings settings(COMPANY, APPNAME);
    //settings.clear();
    settings.setValue("controls_geometry", saveGeometry());
    settings.setValue("videoVolume", ui->volCotrl->value());

    QWidget::closeEvent(bar);
}

void VideoControls::on_volCotrl_valueChanged(int value)
{
    emit changeVolume(value);
}

void VideoControls::on_muteVidButton_clicked()
{
    emit muteButtonClicked();
}
