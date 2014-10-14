#ifndef GEOCOORD_H
#define GEOCOORD_H

#include <QPointF>

class GeoCoord
{
public:
    GeoCoord(const qreal &lat, const qreal &lon);
    QPointF getTilePosition(const int zoomLevel) const;
    QPoint getTilePosition(const int zoomLevel, QPoint &deltaInTile);

    static const GeoCoord fromTilePosition(const QPointF &tilePos, const int zoomLevel);
    qreal lat() const {return m_lat;}
    qreal lon() const {return m_lon;}

private:
    qreal m_lat;
    qreal m_lon;

};

#endif // GEOCOORD_H
