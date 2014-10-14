#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QObject>
#include <QPoint>
#include <QPainter>

class IMapData;

class MapScene : public QObject
{
    Q_OBJECT
public:
    explicit MapScene(QObject *parent = 0);
    void render(QPainter &p, const QRect &rect, IMapData * const mapData);
    void setMinX(const int minX) {m_minX = minX;}
    void setMinY(const int minY) {m_minX = minY;}
    void setMaxX(const int maxX) {m_maxX = maxX;}
    void setMaxY(const int maxY) {m_maxY = maxY;}
    void setZoomLevel(const int zoomLevel) {m_zoomLevel = zoomLevel;}
    void setMapDelta(const QPoint &mapDelta);
    QPoint mapDelta() const {return m_mapOffsetInPx;}

private:
    QRect tileRect(QPoint tilePos, IMapData *mapData);

signals:

public slots:
private:
    int m_minX;
    int m_maxX;
    int m_minY;
    int m_maxY;
    int m_zoomLevel;
    QPoint m_mapOffsetInPx;

};

#endif // MAPSCENE_H
