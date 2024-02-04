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

    index = 0;
}

PlayListItems::~PlayListItems()
{
    delete ui;
    playlist.clear();
    ui_playList.clear();
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
    lastPlayed = currentMedia();
    this->index = index;
}

void PlayListItems::setCurrentIndex(const QString &vidName)
{
    lastPlayed = currentMedia();

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
    lastPlayed = currentMedia();
    index = (index > 0)? index - 1 : playlist.size() - 1;
    return index;
}

int PlayListItems::nextIndex()
{
    lastPlayed = currentMedia();
    int size = playlist.size() - 1;
    index = (index < size)? index + 1 : 0;
    return index;
}

QString PlayListItems::currentMedia()
{
    return playlist[index];
}

QString PlayListItems::lastMedia()
{
    return lastPlayed;
}
