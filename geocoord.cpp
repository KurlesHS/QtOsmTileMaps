#include "geocoord.h"
#include "geoutils.h"

GeoCoord::GeoCoord(const qreal &lat, const qreal &lon) :
    m_lat(lat),
    m_lon(lon)
{

}

QPointF GeoCoord::getTilePosition(const int zoomLevel) const
{
    return tileForCoordinate(lat(), lon(), zoomLevel);
}

QPoint GeoCoord::getTilePosition(const int zoomLevel, QPoint &deltaInTile)
{
    QPointF pos = getTilePosition(zoomLevel);
    QPoint retPos((int)pos.x(), (int)pos.y());
    deltaInTile.setX((pos.x() - retPos.x()) * 0x100);
    deltaInTile.setY((pos.y() - retPos.y()) * 0x100);
    return retPos;
}

const GeoCoord GeoCoord::fromTilePosition(const QPointF &tilePos, const int zoomLevel)
{
    qreal lon = longitudeFromTile(tilePos.x(),  zoomLevel);
    qreal lat = latitudeFromTile(tilePos.y(), zoomLevel);
    return GeoCoord(lat, lon);
}
