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
    //void displayPlayTime(qint64 num);

signals:
    void newItemSelected(QListWidgetItem*);
    void closeAllWindows();
    void changeVolume(int value);

private slots:
    void on_volCtrl_valueChanged(int value);

private:
    Ui::PlayListItems *ui;
    void closeEvent(QCloseEvent *bar);
};

#endif // PLAYLISTITEMS_H
