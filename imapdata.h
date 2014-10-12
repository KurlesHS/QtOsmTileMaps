#ifndef IMAPDATA_H
#define IMAPDATA_H
#include <QImage>
#include <QColor>
#include <QHash>
#include <QMutex>
#include <QList>

// 10000000 / 60000
#define MAX_CACHED_IMAGES 1600

class IMapData
{
protected:
    struct ZoomData {
        int zoomLvl;
        int minX;
        int maxX;
        int minY;
        int maxY;
    };

public:
    IMapData();
    virtual ~IMapData() {}
    virtual QImage getTile(const int x, const int y) = 0;
    QList<int> supportedZoomLevels() const;
    bool zoomUp();
    bool zoomDown();
    void setZoomLvlToMax();
    void setZoomLvlToMin();

    int zoomLvl() const;
    int minZoomLevel() const;
    int maxZoomLevel() const;
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

    QPointF tile2Coordinate(qreal lat, qreal lng);

    double tileX2Long(int x, int offsetXInTile);
    double tileY2Lat(int y, int offsetYInTile);

    int long2TileX(double lon, int *offsetXInTile = 0);
    int lat2TileY(double lat, int *offsetYInTile = 0);

    int tileWidth() const;
    int tileHeight() const;

protected:
    void setZoomLvl(const int zoomLvl);
    void setMinX(const int minX);
    void setMaxX(const int maxX);
    void setMinY(const int minY);
    void setMaxY(const int maxY);
    void setSettingByZoomData(const ZoomData &zd);
    void addZoomLevel(const int &zoomLevel, const ZoomData &zd);

private:
    int m_zoomLvl;
    int m_minX;
    int m_maxX;
    int m_minY;
    int m_maxY;
    int m_numberOfCachedImages;
    int m_maxCachedImages;
    QImage m_defaultBackground;
    QHash<int, ZoomData> m_zoomDatas;
    QHash<QString, QImage> m_tilesHash;
    QMutex m_mutexForTilesHash;
};

#endif // IMAPDATA_H
