#include "zoomanimationrenderer.h"

#include <QTimer>
#include <QDateTime>
#include <QDebug>

ZoomAnimationRenderer::ZoomAnimationRenderer(ISetRenderer *rendererSetter, QObject *parent) :
    IRenderer(rendererSetter, parent)
  ,m_timer(new QTimer(this))
  ,m_animationDuration(350)
  ,m_tickInMs(16)
{
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
}

ZoomAnimationRenderer::~ZoomAnimationRenderer()
{
    m_timer->stop();
    m_timer->deleteLater();
}

void ZoomAnimationRenderer::setMaxTick(const int maxTick)
{
    m_animationDuration = maxTick;
}

void ZoomAnimationRenderer::start()
{
    m_startTime = QDateTime::currentMSecsSinceEpoch();
    m_timer->setSingleShot(true);
    m_timer->start(m_tickInMs);
}

void ZoomAnimationRenderer::onTimeout()
{
    emit needUpdate();
}

void ZoomAnimationRenderer::setTickInMs(const int tickInMs)
{
    m_tickInMs = tickInMs;
}

void ZoomAnimationRenderer::render(QPainter *painter, QRect rect)
{
    int deltatime = QDateTime::currentMSecsSinceEpoch() - m_startTime;
    m_progress = (qreal)deltatime / m_animationDuration;
    bool anotherCycle = true;
    if (m_progress > 1.) {
        m_progress = 1.;
        anotherCycle = false;
    }
    QPoint topLeft = (m_endRect.topLeft() - m_startRect.topLeft()) * m_progress + m_startRect.topLeft();
    QSize size = (m_endRect.size() - m_startRect.size()) * m_progress + m_startRect.size();
    m_currentRect = QRect(topLeft, size);
    renderHandler(painter, rect);
    if (!anotherCycle) {
        m_timer->stop();
        setNextRendererInPainter();
        deleteLater();
    } else {
        int timeout = m_tickInMs - (deltatime % m_tickInMs);
        m_timer->start(timeout);
    }

}
