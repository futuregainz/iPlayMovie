#ifndef VIDEOCONTROLS_H
#define VIDEOCONTROLS_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class VideoControls;
}

class VideoControls : public QWidget
{
    Q_OBJECT

public:
    explicit VideoControls(QWidget *parent = 0);
    ~VideoControls();

    QString getVideoTime(const int &miliseconds);
    QString prependZero(int val);

    const int secToMiliSecs = 1000 * 60;
    const int minToMiliSecs = 1000 * 60 * 60;

    int lastSavedVol = 0;

private slots:
    void on_backButton_clicked();
    void on_playButton_clicked();
    void on_nextButton_clicked();
    void on_videoProgress_sliderMoved(int position);
    void on_volCotrl_valueChanged(int value);
    void on_muteVidButton_clicked();

public slots:
    void displayVideoDuration(qint64 length, qint64 num);
    void setVideoRange(qint64 durantion);
    void videoPaused(bool isPaused);
    void videoMuted(bool isMuted);
    void volumeChanged(int vol);

signals:
    void playPauseVideo();
    void gotoNextVideo();
    void gotoPreviousVideo();
    void changeVolume(int pos);
    void videoSliderMoved(int pos);
    void muteButtonClicked();

protected:
    void closeEvent(QCloseEvent *bar) override;

private:
    Ui::VideoControls *ui;
    bool isVidPaused;
    //bool isVidMuted;
};

#endif // VIDEOCONTROLS_H
