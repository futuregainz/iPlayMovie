#include "playlistitems.h"
#include "ui_playlistitems.h"

PlayListItems::PlayListItems(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayListItems)
{
    ui->setupUi(this);
    ui->listWidget->setCurrentRow(1);

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

void PlayListItems::addPlayListItems(QString item)
{
    ui->listWidget->addItem(item);
}
