#ifndef GEOUTILS_H
#define GEOUTILS_H

#include <QPointF>

QPointF tileForCoordinate(qreal lat, qreal lng, int zoom);
qreal longitudeFromTile(qreal tx, int zoom);
qreal latitudeFromTile(qreal ty, int zoom);

#endif // GEOUTILS_H
