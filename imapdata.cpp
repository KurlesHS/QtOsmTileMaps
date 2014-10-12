#include "imapdata.h"

#include <QMutexLocker>
#include <QDebug>

static const double PI = 3.141592653589793238463;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

QPointF tileForCoordinate(qreal lat, qreal lng, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal tx = (lng + 180.0) / 360.0;
    qreal ty = (1.0 - log(tan(lat * M_PI / 180.0) +
                          1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0;
    return QPointF(tx * zn, ty * zn);
}

qreal longitudeFromTile(qreal tx, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal lat = tx / zn * 360.0 - 180.0;
    return lat;
}

qreal latitudeFromTile(qreal ty, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal n = M_PI - 2 * M_PI * ty / zn;
    qreal lng = 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
    return lng;
}

IMapData::IMapData() :
    m_zoomLvl(0),
    m_minX(0),
    m_maxX(0),
    m_minY(0),
    m_maxY(0),
    m_numberOfCachedImages(0),
    m_maxCachedImages(MAX_CACHED_IMAGES)
{
    //setDefaultBackgroundColor(Qt::green);
    m_defaultBackground.load(":/tiles/resources/no-tile.png");
}

int IMapData::zoomLvl() const
{
    return m_zoomLvl;
}

int IMapData::minZoomLevel() const
{
    auto zoomLevels = supportedZoomLevels();
    if (zoomLevels.empty()) {
        return 0;
    }
    return zoomLevels.at(0);
}

int IMapData::maxZoomLevel() const
{
    auto zoomLevels = supportedZoomLevels();
    if (zoomLevels.empty()) {
        return 0;
    }
    return zoomLevels.at(zoomLevels.size() - 1);
}

int IMapData::minX() const
{
    return m_minX;
}

int IMapData::maxX() const
{
    return m_maxX;
}

int IMapData::minY() const
{
    return m_minY;
}

int IMapData::maxY() const
{
    return m_maxY;
}

QImage IMapData::defaultBackground() const
{
    return m_defaultBackground;
}

void IMapData::setDefaultBackgroundColor(const QColor &color)
{
    m_defaultBackground = QImage(0x100, 0x100, QImage::Format_RGB32);
    m_defaultBackground.fill(color);
}

bool IMapData::putImageInCache(const int x, const int y, const QImage &tile)
{
    QMutexLocker locker(&m_mutexForTilesHash);
    if (m_numberOfCachedImages >= m_maxCachedImages) {
        return false;
    }
    QString hash = QString("%0_%1_%2").arg(zoomLvl()).arg(x).arg(y);
    m_tilesHash[hash] = tile;
    ++m_numberOfCachedImages;
    return true;
}

QImage IMapData::getImageFromCache(const int x, const int y)
{
    QMutexLocker locker(&m_mutexForTilesHash);
    QString hash = QString("%0_%1_%2").arg(zoomLvl()).arg(x).arg(y);
    return m_tilesHash.value(hash, QImage());
}

int IMapData::maxCachedImages() const
{
    return m_maxCachedImages;
}

void IMapData::setMaxCachedImages(int maxCachedImages)
{
    m_maxCachedImages = maxCachedImages;
}

QSize IMapData::mapSizeInPx() const
{
    return QSize((maxX() - minX() + 1) * tileWidth(), (maxY() - minY() + 1) * tileHeight());
}

QList<int> IMapData::supportedZoomLevels() const
{
    QList<int> retVal = m_zoomDatas.keys();
    qSort(retVal);
    return retVal;
}

bool IMapData::zoomUp()
{
    QList<int> zoomLevels = supportedZoomLevels();
    for (const int &zoom : zoomLevels) {
        if (zoom > zoomLvl()) {
            setSettingByZoomData(m_zoomDatas.value(zoom));
            return true;
        }
    }
    return false;
}

bool IMapData::zoomDown()
{
    QList<int> zoomLevels = supportedZoomLevels();
    int idx = zoomLevels.indexOf(zoomLvl());
    if (idx > 0) {
        setSettingByZoomData(m_zoomDatas.value(zoomLevels.at(idx - 1)));
        return true;
    }
    return false;
}

void IMapData::setZoomLvlToMax()
{
    auto zoomLevels = supportedZoomLevels();
    if (zoomLevels.count() > 0) {
        setSettingByZoomData(m_zoomDatas.value(zoomLevels.at(zoomLevels.count() - 1)));
    }
}

void IMapData::setZoomLvlToMin()
{
    auto zoomLevels = supportedZoomLevels();
    if (zoomLevels.count() > 0) {
        setSettingByZoomData(m_zoomDatas.value(zoomLevels.at(0)));
    }
}

#if 0
double tilex2long(int x, int z)
{
    return x / pow(2.0, z) * 360.0 - 180;
}
#endif

QPointF IMapData::tile2Coordinate(qreal lat, qreal lng) {
    return tileForCoordinate(lat, lng, m_zoomLvl) - QPointF(m_minX, m_minY);
}

double IMapData::tileX2Long(int x, int offsetXInTile)
{

    x += m_minX;
    double realPart = offsetXInTile / 256.;
    double tileX = x + realPart;
    return longitudeFromTile(tileX, zoomLvl());
    //return (x + realPart) / pow(2.0, zoomLvl()) * 360.0 - 180;
}

#if 0
double tiley2lat(int y, int z)
{
    double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}
#endif

double IMapData::tileY2Lat(int y, int offsetYInTile)
{
    y += m_minY;
    double realPart = offsetYInTile / 256.;
    double tileY = y + realPart;
    return latitudeFromTile(tileY, zoomLvl());

    //double n = PI - 2.0 * PI * (y + realPart) / pow(2.0, zoomLvl());
    //return 180.0 / PI * atan(0.5 * (exp(n) - exp(-n)));
}

#if 0
int long2tilex(double lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}
#endif

int IMapData::long2TileX(double lon, int *offsetXInTile)
{
    QPointF pos = tileForCoordinate(0, lon, zoomLvl());
    if (offsetXInTile) {
        *offsetXInTile = (pos.x() - floor(pos.x())) * 256;
    }
    return (int) pos.x() - m_minX;

#if 0
    double tileXReal = (lon + 180.0) / 360.0 * pow(2.0, m_zoomLvl);
    int tileX = (int)floor(tileXReal);
    if (offsetXInTile) {
        *offsetXInTile = (int)floor(256 * (tileXReal - tileX));
        qDebug() << Q_FUNC_INFO << *offsetXInTile;
    }
    return tileX - m_minX;
#endif
}

#if 0
int lat2tiley(double lat, int z)
{
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}
#endif

int IMapData::lat2TileY(double lat, int *offsetYInTile)
{
    QPointF pos = tileForCoordinate(lat, 0, zoomLvl());
    if (offsetYInTile) {
        *offsetYInTile = (pos.y() - floor(pos.y())) * 256;
    }
    return (int) pos.y() - m_minY;
#if 0
    double tileYReal =
            (1.0 - log( tan(lat * PI/180.0) + 1.0 / cos(lat * PI/180.0)) / PI)
            / 2.0 * pow(2.0, m_zoomLvl);
    int tileY =  (int)floor(tileYReal);
    if (offsetYInTile) {
        *offsetYInTile = (int)floor(256 * (tileYReal - tileY));
        qDebug() << Q_FUNC_INFO << *offsetYInTile;
    }
    return tileY - m_minY;
#endif
}

int IMapData::tileWidth() const
{
    return 0x0100;
}

int IMapData::tileHeight() const
{
    return 0x0100;
}

void IMapData::setZoomLvl(const int zoomLvl)
{
    ZoomData zoomData;
    memset(&zoomData, 0, sizeof(zoomData));
    if (m_zoomDatas.contains(zoomLvl)) {
        zoomData = m_zoomDatas[zoomLvl];
    } else {
        QList<int> zoomLevels = supportedZoomLevels();
        for (const int &z : zoomLevels) {
            if (zoomLvl < z && zoomData.zoomLvl < z) {
                zoomData = m_zoomDatas.value(z);
            }
        }
        if (zoomData.zoomLvl == 0 && zoomLevels.count() > 0) {
            zoomData = m_zoomDatas.value(0);
        }
    }

    setSettingByZoomData(zoomData);
}

void IMapData::setMinX(const int minX)
{
    m_minX = minX;
}

void IMapData::setMaxX(const int maxX)
{
    m_maxX = maxX;
}

void IMapData::setMinY(const int minY)
{
    m_minY = minY;
}

void IMapData::setMaxY(const int maxY)
{
    m_maxY = maxY;
}

void IMapData::setSettingByZoomData(const IMapData::ZoomData &zd)
{
    m_zoomLvl = zd.zoomLvl;
    m_minX = zd.minX;
    m_maxX = zd.maxX;
    m_minY = zd.minY;
    m_maxY = zd.maxY;
}

void IMapData::addZoomLevel(const int &zoomLevel, const IMapData::ZoomData &zd)
{
    m_zoomDatas[zoomLevel] = zd;
}
