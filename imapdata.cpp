#include "imapdata.h"

#include <QMutexLocker>

static const double PI  =3.141592653589793238463;

IMapData::IMapData(
        const int zoomLvl, const int minX, const int maxX,
        const int minY, const int maxY) :
    m_zoomLvl(zoomLvl),
    m_minX(minX),
    m_maxX(maxX),
    m_minY(minY),
    m_maxY(maxY),
    m_numberOfCachedImages(0),
    m_maxCachedImages(MAX_CACHED_IMAGES)
{
    setDefaultBackgroundColor(Qt::green);
}

int IMapData::zoomLvl() const
{
    return m_zoomLvl;
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
    quint64 hash = x | ((quint64)y << 0x20);
    m_tilesHash[hash] = tile;
    ++m_numberOfCachedImages;
    return true;
}

QImage IMapData::getImageFromCache(const int x, const int y)
{
    QMutexLocker locker(&m_mutexForTilesHash);
    quint64 hash = x | ((quint64)y << 0x20);
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

double IMapData::tileX2Long(int x, int offsetXInTile)
{
    x += m_minX;
    double realPart = offsetXInTile / 256.;
    return (x + realPart) / pow(2.0, zoomLvl()) * 360.0 - 180;
}

double IMapData::tileY2Lat(int y, int offsetYInTile)
{
    y += m_minY;
    double realPart = offsetYInTile / 256.;
    double n = PI - 2.0 * PI * (y + realPart) / pow(2.0, zoomLvl());
    return 180.0 / PI * atan(0.5 * (exp(n) - exp(-n)));
}

int IMapData::long2TileX(double lon, int *offsetXInTile)
{
    double tileXReal = (lon + 180.0) / 360.0 * pow(2.0, m_zoomLvl);
    int tileX = (int)tileXReal;
    if (offsetXInTile) {
        *offsetXInTile = (int) (256 * (tileXReal - tileX));
    }
    return tileX - m_minX;
}

int IMapData::lat2TileY(double lat, int *offsetYInTile)
{
    double tileYReal = (1.0 - log( tan(lat * PI/180.0) + 1.0 / cos(lat * PI/180.0)) / PI) / 2.0 * pow(2.0, m_zoomLvl);
    int tileY =  (int) tileYReal;
    if (offsetYInTile) {
        *offsetYInTile = (int) (256 * (tileYReal - tileY));
    }
    return tileY - m_minY;
}

int IMapData::tileWidth() const
{
    return 0x0100;
}

int IMapData::tileHeight() const
{
    return 0x0100;
}
