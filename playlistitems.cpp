#include "playlistitems.h"
#include "ui_playlistitems.h"

PlayListItems::PlayListItems(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayListItems)
{
    ui->setupUi(this);
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SIGNAL(newItemSelected(QListWidgetItem*)));
}

PlayListItems::~PlayListItems()
{
    delete ui;
}

int PlayListItems::getSelectedItem()
{
    return ui->listWidget->currentRow();
}

QString PlayListItems::getSelectedItemName()
{
    return ui->listWidget->currentItem()->text();
}

QString PlayListItems::getVideoTime(const int &miliseconds)
{
    QString writeTime = "0:0:0";

    QTime time(0,0,0,0);
    time = time.addMSecs(miliseconds);

    if(miliseconds < (1000 * 60)) { // if less than 60 seconds

        writeTime = ":" + QString::number(time.second());
    }
    else if(miliseconds < (1000 * 60 * 60)) {

        writeTime = QString::number(time.minute()) + ":" + QString::number(time.second());
    }
    else { // video is longer than one hour

        writeTime = QString::number(time.hour()) + ":" + QString::number(time.minute()) + ":" + QString::number(time.second());
    }

    return writeTime;
}

void PlayListItems::addPlayListItems(QString item)
{
    ui->listWidget->setCurrentRow(0);
    ui->listWidget->addItem(item);
}

void PlayListItems::updateList(int index)
{
    ui->listWidget->setCurrentRow(index);
}

void PlayListItems::removeListItem()
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

void PlayListItems::displayPlayTime(qint64 length, qint64 num)
{
    QString remaining = getVideoTime((int)length);
    QString full = getVideoTime((int)num);

    ui->vidLength->setText(remaining + " | " + full);
}

void PlayListItems::closeEvent(QCloseEvent *bar)
{
    emit closeAllWindows();
    this->close();
    bar->accept();
}

void PlayListItems::on_volCtrl_valueChanged(int value)
{
    emit changeVolume(value);
}

void PlayListItems::on_hide_Button_clicked()
{
    this->hide();
}
