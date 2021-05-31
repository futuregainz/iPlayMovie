#ifndef PLAYLISTITEMS_H
#define PLAYLISTITEMS_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class PlayListItems;
}

class PlayListItems : public QWidget
{
    Q_OBJECT

public:
    explicit PlayListItems(QWidget *parent = 0);
    ~PlayListItems();

    int getSelectedItem();

public slots:
    void addPlayListItems(QString item);

signals:
    void newItemSelected(QListWidgetItem*);

private:
    Ui::PlayListItems *ui;
};

#endif // PLAYLISTITEMS_H
