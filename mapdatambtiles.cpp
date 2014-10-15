#include "mapdatambtiles.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QDebug>

static const char *databaseConnectionName = "mbtiles-db-connection";

MapDataMbtiles::MapDataMbtiles(const QString &pathToMbtiles, QObject *parent) :
    IMapData(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", databaseConnectionName);
    db.setDatabaseName(pathToMbtiles);
    if (db.open()) {
        QSqlQuery q(db);
        QList<quint8> zoomLevels;
        if (q.exec("select distinct(zoom_level) from map asc")) {
            while (q.next()) {
                zoomLevels.append(q.value(0).toInt());
            }
        }
        for (const quint8 &zoomLevel : zoomLevels) {
            q.prepare("select min(tile_column), max(tile_column), min(tile_row), max(tile_row) from map where zoom_level = ?");
            q.addBindValue(zoomLevel);
            if (q.exec()) {
                while (q.next()) {
                    ZoomData zd;
                    zd.zoomLvl = zoomLevel;
                    zd.minX = q.value(0).toInt();
                    zd.maxX = q.value(1).toInt();
                    zd.minY = q.value(3).toInt();
                    zd.maxY = q.value(2).toInt();
                    zd.minY = reverseYTilePos(zd.minY, zoomLevel);
                    zd.maxY = reverseYTilePos(zd.maxY, zoomLevel);
                    addZoomLevel(zd);
                    if (!isValidData()) {
                        setDataValid(true);
                    }
                }
            }
        }
    }
}

MapDataMbtiles::~MapDataMbtiles()
{
    {
        QSqlDatabase db = QSqlDatabase::database(databaseConnectionName);
        if (db.isOpen()) {
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(databaseConnectionName);
}


QPixmap MapDataMbtiles::getTile(const int x, const int y)
{
    // convert y tile coord to slippy map format (NW origin to SW origin)
    int correctedY = reverseYTilePos(y, zoomLvl());
    if (!checkInBounds(x, y)) {
        return defaultBackground();
    }
    QPixmap retPix = getImageFromCache(x, y);
    if (!retPix.isNull()) {
        return retPix;
    }
    QSqlDatabase db = QSqlDatabase::database(databaseConnectionName);
    QSqlQuery q(db);
    q.prepare("select tile_data from tiles where zoom_level = ? and tile_column = ? and tile_row = ?");
    q.addBindValue(zoomLvl());
    q.addBindValue(x);
    q.addBindValue(correctedY);
    if (q.exec() && q.next()) {
        QByteArray data = q.record().value("tile_data").toByteArray();
        retPix.loadFromData(data);
    }
    if (retPix.height() != tileHeight() || retPix.width() != tileWidth()) {
        retPix = defaultBackground();
    }
    putImageInCache(x, y, retPix);
    return retPix;
}

int MapDataMbtiles::reverseYTilePos(const int yTilePos, const int zoomLevel)
{
    return (1 << zoomLevel) - yTilePos - 1;
}
