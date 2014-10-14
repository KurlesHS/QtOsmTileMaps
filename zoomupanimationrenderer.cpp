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
    m_pixmapForRender = pixmap().copy(currentRect()).scaled(rect.size());
    painter->drawPixmap(rect, m_pixmapForRender);
}
