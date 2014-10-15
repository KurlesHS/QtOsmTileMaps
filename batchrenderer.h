#ifndef BATCHRENDERER_H
#define BATCHRENDERER_H

#include "irenderer.h"
#include <QList>

class BatchRenderer : public IRenderer
{
    Q_OBJECT
public:
    explicit BatchRenderer(ISetRenderer *rendererSetter, QObject *parent = 0);

    // IRenderer interface
    virtual void render(QPainter *painter, QRect rect);

    void addRenderer(IRenderer *renderer);
    void removeRenderer(IRenderer *renderer);

private:
QList<IRenderer*> m_renrerersList;

};

#endif // BATCHRENDERER_H
