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

void PlayListItems::addPlayListItems(QString item)
{
    ui->listWidget->setCurrentRow(0);
    ui->listWidget->addItem(item);
}

void PlayListItems::removeListItem()
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
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
