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

    int lastSavedVol = 0;

private slots:
    void on_backButton_clicked();
    void on_playButton_clicked();
    void on_nextButton_clicked();
    void on_videoProgress_sliderMoved(int position);
    void on_volCotrl_sliderMoved(int position);

public slots:
    void displayVideoDuration(qint64 length, qint64 num);

signals:
    void playPauseVideo();
    void gotoNextVideo();
    void gotoPreviousVideo();
    void changeVolume(int pos);
    void videoSliderMoved(int pos);

protected:
    void closeEvent(QCloseEvent *bar) override;

private:
    Ui::VideoControls *ui;
};

#endif // VIDEOCONTROLS_H
