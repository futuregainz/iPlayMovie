#ifndef QMEDIAPLAYLIST_H
#define QMEDIAPLAYLIST_H

#include "QtCore/qobject.h"
#include <QUrl>
#include <QVector>


class QMediaPlaylist : public QObject
{
public:
    QMediaPlaylist();
    ~QMediaPlaylist();
    int currentIndex();
    void setCurrentIndex(int index);
    void setCurrentIndex(const QString &vidName);
    void addMedia(const QString &item);
    void clear();
    int previousIndex();
    int nextIndex();
    QString currentMedia();
    QString getVideoName(const int &iIndex);

private:
    QVector<QString> playlist;
    int index;

};

#endif // QMEDIAPLAYLIST_H
