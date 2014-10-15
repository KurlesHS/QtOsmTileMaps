#ifndef SLIPPYMAPRENDERER_H
#define SLIPPYMAPRENDERER_H

#include "irenderer.h"

#include <QPixmap>

class IMapData;

class SlippyMapRenderer : public IRenderer
{
    Q_OBJECT
public:
    explicit SlippyMapRenderer(QObject *parent = 0);
    void setMapDataSource(IMapData * const mapDataSource);

    // IRenderer interface
    virtual void render(QPainter *painter, QRect rect);

private:
    QRect tileRect(QPoint tilePos);
    void renderHelper(QPainter *painter, QRect rect);

signals:

public slots:

private:
    IMapData *m_mapDataSource;
    QPoint m_lastMapOffset;
    int m_lastZoomLevel;
    quint64 m_lastTimeChangeCoord;
    QPixmap m_cache;
};

#endif // SLIPPYMAPRENDERER_H
