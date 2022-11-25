#include "playlistitems.h"
#include "ui_playlistitems.h"


PlayListItems::PlayListItems(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayListItems)
{
    ui->setupUi(this);
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SIGNAL(newItemSelected(QListWidgetItem*)));
    connect(ui->searchButton, &QAbstractButton::clicked, this, &PlayListItems::searchForVid);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &PlayListItems::searchForVid);

    ui->listWidget->setCurrentRow(0);

    QSettings settings(COMPANY, APPNAME);
    this->restoreGeometry(settings.value("playlist_geometry").toByteArray());
    lastSavedVol = settings.value("videoVolume").toInt();
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

void PlayListItems::resetPlaylist()
{
    ui->listWidget->clear();
}

void PlayListItems::addPlayListItem(QString item)
{
    ui->listWidget->addItem(item);
    playList.append(item);
}

/*void PlayListItems::updateList(int index)
{
    //ui->listWidget->setCurrentRow(index);
    //ui->listWidget->currentItem()->setBackgroundColor(Qt::darkGreen);
}

void PlayListItems::displayPlayTime(qint64 length, qint64 num)
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

    QSettings settings(COMPANY, APPNAME);
    settings.setValue("playlist_geometry", saveGeometry());
    //settings.setValue("videoVolume", ui->volCtrl->value());

    QWidget::closeEvent(bar);
}

void PlayListItems::searchForVid()
{
    QString searchStr = ui->lineEdit->text().trimmed();
    QStringList results;

    for(int index = 0; index < ui->listWidget->count(); index++)
    {
        QString vid = ui->listWidget->item(index)->text();
        if (vid.contains(searchStr, Qt::CaseInsensitive))
            results.append(vid);
    }

    ui->listWidget->clear();
    ui->listWidget->addItems(results);
}

void PlayListItems::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    item->setBackgroundColor(Qt::darkGreen);
}

void PlayListItems::on_lineEdit_textEdited(const QString &arg1)
{
    if(arg1.trimmed().isEmpty()) {
        ui->listWidget->clear();
        ui->listWidget->addItems(playList);
    }
}
