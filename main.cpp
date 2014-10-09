#include "mainwindowmapsforarm.h"
#include <QApplication>
#include <QDebug>

#include "mapdatadisk.h"
#include "mapwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowMapsForArm w;

    w.show();

    double x = 1.0003;
    double x2 = 1.9999;

    qDebug() << (int) x << (int) x2;

    return a.exec();
}
