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
    mapwidget.cpp \
    imapdata.cpp \
    mapdatadisk.cpp \
    geocoord.cpp \
    geoutils.cpp \
    maprenderer.cpp \
    mapscene.cpp \
    slippymapwidget.cpp \
    slippymaprenderer.cpp \
    zoomanimationrenderer.cpp \
    zoomupanimationrenderer.cpp \
    zoomdownanimationrenderer.cpp

HEADERS  += mainwindowmapsforarm.h \
    mapwidget.h \
    imapdata.h \
    mapdatadisk.h \
    geocoord.h \
    geoutils.h \
    maprenderer.h \
    mapscene.h \
    slippymapwidget.h \
    irenderer.h \
    slippymaprenderer.h \
    DebugMsgWithTime.h \
    zoomanimationrenderer.h \
    isetrenderer.h \
    zoomupanimationrenderer.h \
    zoomdownanimationrenderer.h

FORMS    += mainwindowmapsforarm.ui

RESOURCES += \
    resources.qrc
