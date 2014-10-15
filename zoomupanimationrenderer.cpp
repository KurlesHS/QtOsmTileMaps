#include "zoomupanimationrenderer.h"

#include <QPainter>
#include <QDebug>

ZoomUpAnimationRenderer::ZoomUpAnimationRenderer(
        ISetRenderer *rendererSetter,
        QObject *parent) :
    ZoomAnimationRenderer(rendererSetter, parent)
{
}

void ZoomUpAnimationRenderer::renderHandler(QPainter *painter, QRect rect)
{
    painter->drawPixmap(rect,  pixmap(), currentRect());
}
