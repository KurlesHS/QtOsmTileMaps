#ifndef MAPDATADISK_H
#define MAPDATADISK_H

#include "imapdata.h"

class MapDataDisk : public IMapData
{
    Q_OBJECT
public:
    MapDataDisk(const QString &pathToTiles, QObject *parent = 0);

    // IMapData interface
public:
    virtual QPixmap getTile(int x, int y);

private:
    QString pathAppend(const QString &path1, const QString &path2);
    void parseDir(const QString &pathToTiles);

private:
    QString m_pathToTiles;


};

#endif // MAPDATADISK_H
