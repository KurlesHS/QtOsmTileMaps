#ifndef IMAPDATA_H
#define IMAPDATA_H
#include <QImage>
#include <QColor>
#include <QHash>
#include <QMutex>

// 10000000 / 60000
#define MAX_CACHED_IMAGES 1600

class IMapData
{
public:
    IMapData(const int zoomLvl,
             const int minX, const int maxX,
             const int minY, const int maxY);
    virtual ~IMapData() {}
    int zoomLvl() const;
    int minX() const;
    int maxX() const;
    int minY() const;
    int maxY() const;
    int maxCachedImages() const;
    void setMaxCachedImages(int maxCachedImages);
    QImage defaultBackground() const;
    void setDefaultBackgroundColor(const QColor &color);
    QSize mapSizeInPx() const;

    bool putImageInCache(const int x, const int y, const QImage &tile);
    QImage getImageFromCache(const int x, const int y);

    virtual QImage getTile(const int x, const int y) = 0;
    double tileX2Long(int x, int offsetXInTile);
    double tileY2Lat(int y, int offsetYInTile);

    int long2TileX(double lon, int *offsetXInTile = 0);
    int lat2TileY(double lat, int *offsetYInTile = 0);

    int tileWidth() const;
    int tileHeight() const;


private:
    const int m_zoomLvl;
    const int m_minX;
    const int m_maxX;
    const int m_minY;
    const int m_maxY;
    int m_numberOfCachedImages;
    int m_maxCachedImages;
    QImage m_defaultBackground;
    QHash<quint64, QImage> m_tilesHash;
    QMutex m_mutexForTilesHash;
};

#endif // IMAPDATA_H
