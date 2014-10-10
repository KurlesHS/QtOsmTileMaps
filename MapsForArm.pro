#-------------------------------------------------
#
# Project created by QtCreator 2014-10-09T09:24:24
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MapsForArm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindowmapsforarm.cpp \
    mapwidget.cpp \
    imapdata.cpp \
    mapdatadisk.cpp

HEADERS  += mainwindowmapsforarm.h \
    mapwidget.h \
    imapdata.h \
    mapdatadisk.h

FORMS    += mainwindowmapsforarm.ui

RESOURCES += \
    resources.qrc
