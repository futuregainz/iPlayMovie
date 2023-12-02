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

    index = 0;

}

PlayListItems::~PlayListItems()
{
    delete ui;
    playlist.clear();
    ui_playList.clear();
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

void PlayListItems::playlistClear()
{
    ui->listWidget->clear();
    ui_playList.clear();
    playlist.clear();
}

/*void PlayListItems::removeItem(int row)
{
    ui->listWidget->takeItem(row);
    ui_playList.removeAt(row);
}

void PlayListItems::renameItem(int row, const QString &newName)
{
    ui->listWidget->clear();
    ui_playList[row] = newName;
    ui->listWidget->addItems(ui_playList);
}*/

void PlayListItems::addPlayListItem(QString item)
{
    ui->listWidget->addItem(item);
    ui_playList.push_back(item);
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

    for (int index = 0; index < ui->listWidget->count(); index++)
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
    item->setBackground(Qt::darkGreen);
}

void PlayListItems::on_lineEdit_textEdited(const QString &arg1)
{
    if (arg1.trimmed().isEmpty())
    {
        ui->listWidget->clear();
        ui->listWidget->addItems(ui_playList);
    }
}

int PlayListItems::currentIndex()
{
    return index;
}

void PlayListItems::setCurrentIndex(int index)
{
    this->index = index;
}

void PlayListItems::setCurrentIndex(const QString &vidName)
{
    for (int i = 0; i < playlist.size(); i++)
    {
        if (playlist[i] == vidName)
        {
            this->index = i;
            break;
        }
    }
}

void PlayListItems::addMedia(const QString &item)
{
    playlist.push_back(item);
}

int PlayListItems::previousIndex()
{
    index = (index > 0)? index - 1 : 0;
    return index;
}

int PlayListItems::nextIndex()
{
    int size = playlist.length() - 1;
    index = (index < size)? index + 1 : size;
    return index;
}

QString PlayListItems::currentMedia()
{
    return playlist[index];
}

QString PlayListItems::getVideoName(const int &iIndex)
{
    return playlist[iIndex];
}

/*void PlayListItems::removeMedia(const int &index)
{
    playlist.removeAt(index);
}

void PlayListItems::renameMedia(const int &index, const QString &newName)
{
    playlist[index] = newName;
}*/
