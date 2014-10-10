#include "mapdatadisk.h"
#include <QDir>
#include <QDebug>
#include <QRegExp>

MapDataDisk::MapDataDisk(const QString &pathToTiles) :
    IMapData(),
    m_pathToTiles(pathToTiles)
{
    QDir path(pathToTiles);
    QRegExp regExpForYCoord("^(\\d+)\\.png$");
    for (const QString zoomLvlStr : path.entryList(QDir::Dirs)) {
        bool ok;
        int zoomLvl = zoomLvlStr.toInt(&ok);
        if (ok) {
            QDir pathToXCoord(pathAppend(pathToTiles, zoomLvlStr));
            ZoomData zd;
            zd.zoomLvl = zoomLvl;
            zd.maxX = INT_MIN;
            zd.minX = INT_MAX;
            zd.maxY = INT_MIN;
            zd.minY = INT_MAX;
            bool firstEntry = true;
            for (const QString &xStr : pathToXCoord.entryList(QDir::Dirs)) {
                int x = xStr.toInt(&ok);
                if (ok) {
                    if (x < zd.minX) {
                        zd.minX = x;
                    }
                    if (x > zd.maxX) {
                        zd.maxX = x;
                    }
                }
                if (firstEntry && ok) {
                    firstEntry = false;
                    QDir pathToYCoord(pathAppend(pathToXCoord.absolutePath(), xStr));
                    for (const QString &yStr : pathToYCoord.entryList({QString("*.png")}, QDir::Files)) {
                        if (regExpForYCoord.indexIn(yStr) >= 0) {
                            int y = regExpForYCoord.cap(1).toInt(&ok);
                            if (ok) {
                                if (y < zd.minY) {
                                    zd.minY = y;
                                }
                                if (y > zd.maxY) {
                                    zd.maxY = y;
                                }
                            }
                        }
                    }
                }
            }
            if (zd.maxX != INT_MIN && zd.maxY != INT_MIN && zd.minX != INT_MAX && zd.minY != INT_MAX) {
                addZoomLevel(zoomLvl, zd);
            }
        }
    }
}


QImage MapDataDisk::getTile(int x, int y)
{
    if (x > (maxX() - minY()) || x < 0 || y > (maxY() - minY()) || y < 0) {
        return defaultBackground();
    }
    QImage tile = getImageFromCache(x, y);
    if (tile.isNull()) {
        x += minX();
        y += minY();
        QString filename = QString("%1/%2/%3.png")
                .arg(zoomLvl())
                .arg(x)
                .arg(y);
        QString filepath = pathAppend(m_pathToTiles, filename);
        tile.load(filepath);
    } else {
        return tile;
    }
    if (tile.isNull() || tile.height() != 0x100 || tile.width() != 0x100) {
        return defaultBackground();
    }
    putImageInCache(x - minX(), y - minY(), tile);
    return tile;
}

QString MapDataDisk::pathAppend(const QString& path1, const QString& path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}
