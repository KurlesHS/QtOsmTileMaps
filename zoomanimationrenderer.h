#ifndef ZOOMANIMATIONRENDERER_H
#define ZOOMANIMATIONRENDERER_H

#include "irenderer.h"

#include <QPixmap>

class QTimer;

class ZoomAnimationRenderer : public IRenderer
{
    Q_OBJECT
public:
    explicit ZoomAnimationRenderer(ISetRenderer *rendererSetter, QObject *parent = 0);
    ~ZoomAnimationRenderer();
    void setMaxTick(const int maxTick);
    void start();
    void setPixmap(const QPixmap &pixmap) {m_pixmap = pixmap;}
    void setStartRect(const QRect &startRect) {m_startRect = startRect;}
    void setEndRect(const QRect &endRect) {m_endRect = endRect;}
    void setTickInMs(const int tickInMs);
    qreal progress() const {return m_progress;}

    // IRenderer interface
    virtual void render(QPainter *painter, QRect rect);

protected:
    virtual void renderHandler(QPainter *painter, QRect rect) = 0;
    QRect startRect() const {return m_startRect;}
    QRect endRect() const {return m_endRect;}
    QRect currentRect() const {return m_currentRect;}
    QPixmap pixmap() const {return m_pixmap;}

private Q_SLOTS:
    void onTimeout();

private:
    QTimer *m_timer;
    QRect m_startRect;
    QRect m_endRect;
    QRect m_currentRect;
    int m_startTime;
    int m_animationDuration;
    int m_tickInMs;
    qreal m_progress;
    quint64 m_currentMs;
    QPixmap m_pixmap;



};

#endif // ZOOMANIMATIONRENDERER_H
