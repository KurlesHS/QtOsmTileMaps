#ifndef SLIPPYMAPRENDERER_H
#define SLIPPYMAPRENDERER_H

#include "irenderer.h"

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

signals:

public slots:

private:
    IMapData *m_mapDataSource;
};

#endif // SLIPPYMAPRENDERER_H
