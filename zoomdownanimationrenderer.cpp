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
    QPoint topLeft = (startRect().topLeft() - endRect().topLeft()) * progress() + endRect().topLeft();
    QSize size = (startRect().size() - endRect().size()) * progress() + endRect().size();
    painter->drawPixmap(rect, m_pixmapTo, QRect(topLeft, size));
    painter->drawPixmap(currentRect(), pixmap(), pixmap().rect());
}
