#ifndef PLAYLISTITEMS_H
#define PLAYLISTITEMS_H

#include <QWidget>
#include <QListWidgetItem>
#include <QCloseEvent>

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
    QString getSelectedItemName();

public slots:
    void addPlayListItems(QString item);
    void removeListItem();

signals:
    void newItemSelected(QListWidgetItem*);
    void closeAllWindows();

private:
    Ui::PlayListItems *ui;
    void closeEvent(QCloseEvent *bar);
};

#endif // PLAYLISTITEMS_H
