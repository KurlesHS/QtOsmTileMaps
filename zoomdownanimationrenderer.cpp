#include "zoomdownanimationrenderer.h"

#include <QPainter>

ZoomDownAnimationRenderer::ZoomDownAnimationRenderer(
        ISetRenderer *rendererSetter,
        QObject *parent) :
    ZoomAnimationRenderer(rendererSetter, parent)
{
}

void ZoomDownAnimationRenderer::renderHandler(QPainter *painter, QRect rect)
{
    m_pixmapForRender = pixmap().scaled(currentRect().size());
    painter->fillRect(rect, Qt::gray);
    painter->drawPixmap(currentRect(), m_pixmapForRender);
}
