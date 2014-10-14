#include "zoomanimationrenderer.h"

#include <QTimer>
#include <QDateTime>
#include <QDebug>

ZoomAnimationRenderer::ZoomAnimationRenderer(ISetRenderer *rendererSetter, QObject *parent) :
    IRenderer(rendererSetter, parent)
  ,m_timer(new QTimer(this))
  ,m_maxTick(10)
  ,m_tickInMs(25)
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
    m_maxTick = maxTick;
}

void ZoomAnimationRenderer::start()
{
    m_currentTick = 0;
    m_timer->setSingleShot(true);
    m_timer->start(m_tickInMs);

}

void ZoomAnimationRenderer::onTimeout()
{
    if (m_currentTick > m_maxTick) {
        deleteLater();
        m_timer->stop();
        if (rendererSetter()) {
            rendererSetter()->setRenderer(nextRenderer());
        }
    } else {
        m_currentMs = QDateTime::currentMSecsSinceEpoch();
        qreal progress = (qreal)m_currentTick++ / m_maxTick;
        QPoint topLeft = (m_endRect.topLeft() - m_startRect.topLeft()) * progress + m_startRect.topLeft();
        QSize size = (m_endRect.size() - m_startRect.size()) * progress + m_startRect.size();
        m_currentRect = QRect(topLeft, size);
    }
    emit needUpdate();
}

void ZoomAnimationRenderer::setTickInMs(const int tickInMs)
{
    m_tickInMs = tickInMs;
}



void ZoomAnimationRenderer::render(QPainter *painter, QRect rect)
{
    renderHandler(painter, rect);
    int deltaT = QDateTime::currentMSecsSinceEpoch() - m_currentMs;
    m_currentTick += deltaT / m_tickInMs;
    m_timer->start(m_tickInMs - (deltaT % m_tickInMs));
}
