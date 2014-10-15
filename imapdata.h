#ifndef IMAPDATA_H
#define IMAPDATA_H
#include <QPixmap>
#include <QColor>
#include <QHash>
#include <QMutex>
#include <QList>
#include <QMetaType>
#include <QPixmapCache>

// 10000000 / 60000
#define MAX_CACHED_IMAGES 300

class PixmapCache {
public:
    PixmapCache(int maxSize);
    void putImageInCache(const QString &key, const QPixmap &pixmap);
    QPixmap getImageFromCache(const QString &key);

private:
    const int m_maxSize;
    QList<QString> m_keys;
    QHash<QString, QPixmap> m_pixmapCache;
};

class IMapData : public QObject
{
    Q_OBJECT
protected:
    struct ZoomData {
        int zoomLvl;
        int minX;
        int maxX;
        int minY;
        int maxY;
    };

public:
    IMapData(QObject *parent = nullptr);
    virtual ~IMapData() {}
    virtual QPixmap getTile(const int x, const int y) = 0;
    QPixmap getTile(const QPoint &tilePos);
    QList<int> supportedZoomLevels() const;

    int zoomLvl() const;
    int minZoomLevel() const;
    int maxZoomLevel() const;
    int minX() const;
    int maxX() const;
    int minY() const;
    int maxY() const;
    QRect tileBounds() const;
    bool canZoomUp() const;
    bool canZoomDown() const;
    int maxCachedImages() const;
    void setMaxCachedImages(int maxCachedImages);
    QPixmap defaultBackground() const;
    void setDefaultBackgroundColor(const QColor &color);
    QSize mapSizeInPx() const;

    bool putImageInCache(const int x, const int y, const QPixmap &tile);
    QPixmap getImageFromCache(const int x, const int y);

    int tileWidth() const;
    int tileHeight() const;

    QPoint currentMapOffset() const;
    void setCurrentMapOffset(const QPoint &currentMapOffset);

    void adjustMap(const QRect &rect, const int newZoomLevel, QPoint adjustTo = QPoint(-1, -1));
    void toTopLeftCorner();

signals:
    void requestedTile(const int tileX, const int tileY, const QPixmap &tileImage);

public Q_SLOTS:
    void requestTile(const int tileX, const int tileY);
    bool zoomUp();
    bool zoomDown();
    void setZoomLvlToMax();
    void setZoomLvlToMin();

protected:
    void setZoomLvl(const int zoomLvl);
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
    QPixmap m_defaultBackground;
    QHash<int, ZoomData> m_zoomDatas;
    QMutex m_mutexForTilesHash;
    QPoint m_currentMapOffset;
    PixmapCache m_pixmapCache;
};

#endif // IMAPDATA_H
