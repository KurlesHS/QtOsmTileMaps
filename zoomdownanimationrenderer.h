#include "zoomanimationrenderer.h"

#ifndef ZOOMDOWNANIMATIONRENDERER_H
#define ZOOMDOWNANIMATIONRENDERER_H

class ZoomDownAnimationRenderer : public ZoomAnimationRenderer
{
    Q_OBJECT
public:
    explicit ZoomDownAnimationRenderer(ISetRenderer *rendererSetter, QObject *parent = 0);
    void setPixmapTo(const QPixmap &pixmapTo) {m_pixmapTo = pixmapTo;}

    // ZoomAnimationRenderer interface
protected:
    virtual void renderHandler(QPainter *painter, QRect rect);

private:
    QPixmap m_pixmapTo;


};

#endif // ZOOMDOWNANIMATIONRENDERER_H
