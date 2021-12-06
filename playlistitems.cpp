#include "playlistitems.h"
#include "ui_playlistitems.h"


PlayListItems::PlayListItems(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayListItems)
{
    ui->setupUi(this);
    connect(ui->volCtrl, SIGNAL(valueChanged(int)), this, SIGNAL(changeVolume(int)));
    //connect(ui->videoTimePassed, SIGNAL(sliderMoved(int)), this, SIGNAL(videoSliderMoved(int)));
    connect(ui->playPause, SIGNAL(clicked()), this, SIGNAL(playPauseVideo()));
    connect(ui->playNext, SIGNAL(clicked()), this, SIGNAL(gotoNextVideo()));
    connect(ui->goBack, SIGNAL(clicked()), this, SIGNAL(gotoPreviousVideo()));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SIGNAL(newItemSelected(QListWidgetItem*)));

    ui->listWidget->setCurrentRow(0);

    QSettings settings("NunyaBiz", "iPlay Movie");
    this->restoreGeometry(settings.value("playlist/geometry").toByteArray());
    lastSavedVol = settings.value("videoVolume").toInt();
    ui->volCtrl->setValue(lastSavedVol);
}

PlayListItems::~PlayListItems()
{
    delete ui;
}

int PlayListItems::lastSavedVolume()
{
    return lastSavedVol;
}

int PlayListItems::getSelectedItem()
{
    return ui->listWidget->currentRow();
}

QString PlayListItems::getItemName()
{
    return ui->listWidget->currentItem()->text();
}

QString PlayListItems::getItemName(int index)
{
    return ui->listWidget->item(index)->text();
}

QString PlayListItems::getVideoTime(const int &miliseconds)
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

QString PlayListItems::prependZero(int val)
{
    return (val < 10)? "0" + QString::number(val) : QString::number(val);
}

void PlayListItems::resetPlaylist()
{
    ui->listWidget->clear();
}

void PlayListItems::addPlayListItems(QString item)
{
    ui->listWidget->addItem(item);
}

void PlayListItems::updateList(int index)
{
    ui->listWidget->setCurrentRow(index);
    ui->listWidget->currentItem()->setBackgroundColor(Qt::darkGreen);
}

/*void PlayListItems::displayPlayTime(qint64 length, qint64 num)
{
    //int timeRemaining = num - length;
    //QString remaining = getVideoTime(timeRemaining);

    ui->videoTimePassed->setRange(0, num);
    ui->videoTimePassed->setValue(length);

    //ui->vidLength->setText(remaining);
}*/

void PlayListItems::closeEvent(QCloseEvent *bar)
{
    this->close();

    QSettings settings("NunyaBiz", "iPlay Movie");
    settings.setValue("playlist/geometry", saveGeometry());
    settings.setValue("videoVolume", ui->volCtrl->value());

    QWidget::closeEvent(bar);
}

void PlayListItems::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    item->setBackgroundColor(Qt::darkGreen);
}

