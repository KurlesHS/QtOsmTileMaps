#include "mapdatadisk.h"
#include <QDir>
#include <QDebug>

MapDataDisk::MapDataDisk(const QString &pathToTiles, const int zoomLvl,
                         const int minX, const int maxX,
                         const int minY, const int maxY) :
    IMapData(zoomLvl, minX, maxX, minY, maxY),
    m_pathToTiles(pathToTiles)
{
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
