#ifndef IRENDERER_H
#define IRENDERER_H

#include "isetrenderer.h"

#include <QObject>
#include <QRect>

class QPainter;

class IRenderer : public QObject
{
    Q_OBJECT
public:
    explicit IRenderer(ISetRenderer *rendererSetter, QObject *parent = 0) :
        QObject(parent),
        m_nextRenderer(nullptr),
        m_rendererSetter(rendererSetter) {}
    virtual ~IRenderer() {}
    virtual void render(QPainter *painter, QRect rect) = 0;
    virtual void setNextRenderer(IRenderer *renderer) {m_nextRenderer = renderer;}
    virtual IRenderer *nextRenderer() {return m_nextRenderer;}
    ISetRenderer *rendererSetter() const {return m_rendererSetter;}

protected:
    void setNextRendererInPainter () {
        if (m_rendererSetter) {
            m_rendererSetter->setRenderer(nextRenderer());
            setNextRenderer(nullptr);
        }
    }

signals:
    void needUpdate();

public slots:

private:
    IRenderer *m_nextRenderer;
    ISetRenderer *m_rendererSetter;

};

#endif // IRENDERER_H
