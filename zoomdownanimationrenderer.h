#include "zoomanimationrenderer.h"

#ifndef ZOOMDOWNANIMATIONRENDERER_H
#define ZOOMDOWNANIMATIONRENDERER_H

class ZoomDownAnimationRenderer : public ZoomAnimationRenderer
{
    Q_OBJECT
public:
    explicit ZoomDownAnimationRenderer(ISetRenderer *rendererSetter, QObject *parent = 0);

    // ZoomAnimationRenderer interface
protected:
    virtual void renderHandler(QPainter *painter, QRect rect);

private:
    QPixmap m_pixmapForRender;


};

#endif // ZOOMDOWNANIMATIONRENDERER_H
