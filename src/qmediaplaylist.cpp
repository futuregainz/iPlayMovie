#include "qmediaplaylist.h"

QMediaPlaylist::QMediaPlaylist()
{
    index = 0;
}

QMediaPlaylist::~QMediaPlaylist()
{
    playlist.clear();
}

int QMediaPlaylist::currentIndex()
{
    return index;
}

void QMediaPlaylist::setCurrentIndex(int index)
{
    this->index = index;
}

void QMediaPlaylist::setCurrentIndex(const QString &vidName)
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

void QMediaPlaylist::addMedia(const QString &item)
{
    playlist.push_back(item);
}

void QMediaPlaylist::clear()
{
    playlist.clear();
}

int QMediaPlaylist::previousIndex()
{
    index = (index > 0)? index - 1 : 0;
    return index;
}

int QMediaPlaylist::nextIndex()
{
    int size = playlist.length() - 1;
    index = (index < size)? index + 1 : size;
    return index;
}

QString QMediaPlaylist::currentMedia()
{
    return playlist[index];
}

QString QMediaPlaylist::getVideoName(const int &iIndex)
{
    return playlist[iIndex];
}
