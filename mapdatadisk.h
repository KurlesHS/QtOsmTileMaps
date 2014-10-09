#ifndef MAPDATADISK_H
#define MAPDATADISK_H

#include "imapdata.h"

class MapDataDisk : public IMapData
{
public:
    MapDataDisk(const QString &pathToTiles, const int zoomLvl,
                const int minX, const int maxX,
                const int minY, const int maxY);

    // IMapData interface
public:
    virtual QImage getTile(int x, int y);

private:
    QString pathAppend(const QString &path1, const QString &path2);

private:
    QString m_pathToTiles;


};

#endif // MAPDATADISK_H
