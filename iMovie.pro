#-------------------------------------------------
#
# Project created by QtCreator 2021-05-23T20:52:40
#
#-------------------------------------------------

QT       += core gui sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iPlayMovie
TEMPLATE = app


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS_WARN_ON = -Wall #-Wno-unused-parameter

CONFIG+=sdk_no_version_check

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
        src/handleloging.cpp \
        src/movieplayer.cpp \
        src/playlistitems.cpp \
        src/qmediaplaylist.cpp \
        src/videocontrols.cpp \
        src/videosettings.cpp

HEADERS += \
        src/mainwindow.h \
        src/handleloging.h \
        src/movieplayer.h \
        src/qmediaplaylist.h \
        src/text.h \
        src/playlistitems.h \
        src/videocontrols.h \
        src/videosettings.h

FORMS += \
        src/mainwindow.ui \
        src/handleloging.ui \
        src/movieplayer.ui \
        src/playlistitems.ui \
        src/videocontrols.ui

RESOURCES += \
        images.qrc

DISTFILES += \
    README.md
