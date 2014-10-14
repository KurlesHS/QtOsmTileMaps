#include "mainwindowmapsforarm.h"
#include <QApplication>
#include <QDebug>
#include <geocoord.h>

#include "mapdatadisk.h"
#include "mapwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowMapsForArm w;
    w.show();
#if 0
    GeoCoord x1 = GeoCoord::fromTilePosition(QPointF(618, 301), 10);
    GeoCoord x2 = GeoCoord::fromTilePosition(QPointF(1237, 603), 11);
    GeoCoord x3 = GeoCoord::fromTilePosition(QPointF(2475, 1207), 12);
#endif
    return a.exec();
}
