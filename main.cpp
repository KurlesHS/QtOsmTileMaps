#include "mainwindowmapsforarm.h"
#include <QApplication>
#include <QDebug>
#include <geocoord.h>

#include "mapdatadisk.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString path;
    if (argc > 1) {
        path = argv[1];
    }

    MainWindowMapsForArm w(path);
    w.show();
    return a.exec();
}
