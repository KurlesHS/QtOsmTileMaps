#include "batchrenderer.h"

BatchRenderer::BatchRenderer(
        ISetRenderer *rendererSetter,
        QObject *parent) :
    IRenderer(rendererSetter, parent)
{
}

void BatchRenderer::render(QPainter *painter, QRect rect)
{
    for (IRenderer * renderer : m_renrerersList) {
        renderer->render(painter, rect);
    }
}

void BatchRenderer::addRenderer(IRenderer *renderer)
{
    m_renrerersList.append(renderer);
}

void BatchRenderer::removeRenderer(IRenderer *renderer)
{
    m_renrerersList.removeAll(renderer);
}
