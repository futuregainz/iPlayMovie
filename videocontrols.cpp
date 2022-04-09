#include "videocontrols.h"
#include "ui_videocontrols.h"

#include <QSettings>
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
}

VideoControls::~VideoControls()
{
    delete ui;
}

/*<<>> Click on video to use shortcuts <<>>
X: Exit  | L: Loop | D: Delete |  Space: Pause |
> : Seek  |  ^: Next | M: Mute  |  F: FullScreen |
R: Restart Video | N: Rename Video*/ //add these controls


void VideoControls::on_backButton_clicked()
{
    emit gotoPreviousVideo();
}

void VideoControls::on_playButton_clicked()
{
    emit playPauseVideo();
    (ui->playButton->text() == "Play")? ui->playButton->setText("Pause") :
                                        ui->playButton->setText("Play");
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

QString VideoControls::getVideoTime(const int &miliseconds)
{
    QString writeTime = "0:0:0";

    QTime time(0,0,0,0);
    time = time.addMSecs(miliseconds);

    if(miliseconds < (1000 * 60)) { // if less than 60 seconds

        writeTime = "00:" + prependZero(time.second());
    }
    else if(miliseconds < (1000 * 60 * 60)) {

        writeTime = prependZero(time.minute()) + ":" + prependZero(time.second());
    }
    else { // video is longer than one hour

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
    settings.setValue("controls_geometry", saveGeometry());
    settings.setValue("videoVolume", ui->volCotrl->value());

    QWidget::closeEvent(bar);
}

void VideoControls::on_reduceVol_clicked()
{
    if (ui->volCotrl->value() == 0)
        return;
    int vol = ui->volCotrl->value() - 2;
    emit changeVolume(vol);
    ui->volCotrl->setValue(vol);
}

void VideoControls::on_increaseVol_clicked()
{
    int vol = ui->volCotrl->value() + 2;
    emit changeVolume(vol);
    ui->volCotrl->setValue(vol);
}
