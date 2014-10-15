#-------------------------------------------------
#
# Project created by QtCreator 2014-10-09T09:24:24
#
#-------------------------------------------------

QT       += core gui opengl
QMAKE_CXXFLAGS += -std=c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MapsForArm
TEMPLATE = app

include($$PWD/json/qt-json.pri)


SOURCES += main.cpp\
        mainwindowmapsforarm.cpp \
    imapdata.cpp \
    mapdatadisk.cpp \
    geocoord.cpp \
    geoutils.cpp \
    slippymapwidget.cpp \
    slippymaprenderer.cpp \
    zoomanimationrenderer.cpp \
    zoomupanimationrenderer.cpp \
    zoomdownanimationrenderer.cpp \
    batchrenderer.cpp

HEADERS  += mainwindowmapsforarm.h \
    imapdata.h \
    mapdatadisk.h \
    geocoord.h \
    geoutils.h \
    slippymapwidget.h \
    irenderer.h \
    slippymaprenderer.h \
    zoomanimationrenderer.h \
    isetrenderer.h \
    zoomupanimationrenderer.h \
    zoomdownanimationrenderer.h \
    debugmsgwithtime.h \
    batchrenderer.h

FORMS    += mainwindowmapsforarm.ui

RESOURCES += \
    resources.qrc
