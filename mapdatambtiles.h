#ifndef MAPDATAMBTILES_H
#define MAPDATAMBTILES_H

#include "imapdata.h"

class MapDataMbtiles : public IMapData
{
    Q_OBJECT
public:
    explicit MapDataMbtiles(const QString &pathToMbtiles, QObject *parent = 0);
    ~MapDataMbtiles();

    // IMapData interface
public:
    QPixmap getTile(const int x, const int y);

private:
    int reverseYTilePos(const int yTilePos, const int zoomLevel);

};

#endif // MAPDATAMBTILES_H
