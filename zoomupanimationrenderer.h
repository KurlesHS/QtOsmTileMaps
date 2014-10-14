#ifndef ZOOMUPANIMATIONRENDERER_H
#define ZOOMUPANIMATIONRENDERER_H

#include "zoomanimationrenderer.h"

class ZoomUpAnimationRenderer : public ZoomAnimationRenderer
{
    Q_OBJECT
public:
    explicit ZoomUpAnimationRenderer(ISetRenderer *rendererSetter, QObject *parent = 0);

    // ZoomAnimationRenderer interface
protected:
    virtual void renderHandler(QPainter *painter, QRect rect);

private:
    QPixmap m_pixmapForRender;
};

#endif // ZOOMUPANIMATIONRENDERER_H
