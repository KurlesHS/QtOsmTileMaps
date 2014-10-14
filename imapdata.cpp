#include "imapdata.h"
#include "geocoord.h"

#include <QMutexLocker>
#include <QDebug>

IMapData::IMapData(QObject *parent) :
    QObject(parent),
    m_zoomLvl(0),
    m_minX(0),
    m_maxX(0),
    m_minY(0),
    m_maxY(0),
    m_numberOfCachedImages(0),
    m_maxCachedImages(MAX_CACHED_IMAGES)
{
    m_defaultBackground.load(":/tiles/resources/no-tile.png");
}

QPixmap IMapData::getTile(const QPoint &tilePos)
{
    return getTile(tilePos.x(), tilePos.y());
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

QRect IMapData::tileBounds() const
{
    return QRect(minX(), minY(), maxX(), maxY());
}

bool IMapData::canZoomUp() const
{
    auto s = supportedZoomLevels();
    if (s.count() > 0 && s.at(s.count() - 1) > zoomLvl()) {
        return true;
    }
    return false;
}

bool IMapData::canZoomDown() const
{
    auto s = supportedZoomLevels();
    if (s.count() > 0 && s.at(0) < zoomLvl()) {
        return true;
    }
    return false;
}

QPixmap IMapData::defaultBackground() const
{
    return m_defaultBackground;
}

void IMapData::setDefaultBackgroundColor(const QColor &color)
{
    m_defaultBackground = QPixmap(0x100, 0x100);
    m_defaultBackground.fill(color);
}

bool IMapData::putImageInCache(const int x, const int y, const QPixmap &tile)
{
    QMutexLocker locker(&m_mutexForTilesHash);
    if (m_numberOfCachedImages >= m_maxCachedImages) {
        return false;
    }
    QString hash = QString("%0_%1_%2").arg(zoomLvl()).arg(x).arg(y);
    QPixmapCache::insert(hash, tile);
    ++m_numberOfCachedImages;
    return true;
}

QPixmap IMapData::getImageFromCache(const int x, const int y)
{
    QMutexLocker locker(&m_mutexForTilesHash);
    QString hash = QString("%0_%1_%2").arg(zoomLvl()).arg(x).arg(y);
    QPixmap retVal;
    QPixmapCache::find(hash, retVal);
    return retVal;
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

int IMapData::tileWidth() const
{
    return 0x0100;
}

int IMapData::tileHeight() const
{
    return 0x0100;
}

void IMapData::requestTile(const int tileX, const int tileY)
{
    QPixmap tileImage = getTile(tileX, tileY);
    emit requestedTile(tileX, tileY, tileImage);
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
QPoint IMapData::currentMapOffset() const
{
    return m_currentMapOffset;
}

void IMapData::setCurrentMapOffset(const QPoint &currentMapOffset)
{
    m_currentMapOffset = currentMapOffset;
}

void IMapData::adjustMap(const QRect &rect, const int newZoomLevel, QPoint adjustTo)
{
    if (!supportedZoomLevels().contains(newZoomLevel)) {
        return;
    }
    if (adjustTo.x() < 0 || adjustTo.y() < 0) {
        adjustTo = QPoint(rect.size().width() / 2, rect.size().height() / 2);
    }
    qreal tileX = (m_currentMapOffset.x() + adjustTo.x()) / (qreal)tileWidth();
    qreal tileY = (m_currentMapOffset.y() + adjustTo.y()) / (qreal)tileHeight();
    GeoCoord centerToCoord = GeoCoord::fromTilePosition(QPointF(tileX, tileY), this->zoomLvl());
    QPointF newCenter = centerToCoord.getTilePosition(newZoomLevel);
    newCenter *= (qreal)tileHeight();
    setZoomLvl(newZoomLevel);
    QPointF xxx = newCenter - adjustTo;
    m_currentMapOffset = xxx.toPoint();
}

void IMapData::toTopLeftCorner()
{
    m_currentMapOffset = tileBounds().topLeft() * tileWidth();
}

