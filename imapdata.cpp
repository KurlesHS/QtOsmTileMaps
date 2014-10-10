#include "imapdata.h"

#include <QMutexLocker>

static const double PI  =3.141592653589793238463;

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
