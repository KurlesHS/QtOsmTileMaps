#include "slippymapwidget.h"
#include "imapdata.h"
#include "slippymaprenderer.h"
#include "zoomdownanimationrenderer.h"
#include "zoomupanimationrenderer.h"

#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

SlippyMapWidget::SlippyMapWidget(QWidget *parent) :
    QGLWidget(parent),
    m_maxFrameRate(10),
    m_maxFrameRateWhenMoveAnimation(60),
    m_mapDataSource(nullptr),
    m_isLeftMousePressed(false),
    m_isUserActionsEnabled(true),
    m_slippyMapRenderer(new SlippyMapRenderer(this)),
    m_mainBatchRenderer(new BatchRenderer(this, this)),
    m_currentRenderer(m_slippyMapRenderer),
    m_updateTimer(new QTimer(this))

{
    setAutoFillBackground(false);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_mainBatchRenderer->addRenderer(m_slippyMapRenderer);
    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    m_updateTimer->start(timerInterval());
}

SlippyMapWidget::~SlippyMapWidget()
{
    m_updateTimer->stop();
}

void SlippyMapWidget::setMapDataSource(IMapData * const mapDataSource)
{
    if (m_mapDataSource) {
        delete m_mapDataSource;
    }
    m_mapDataSource = mapDataSource;
    m_slippyMapRenderer->setMapDataSource(mapDataSource);
}

void SlippyMapWidget::zoomUp(const QPoint &centerTo)
{
    if (m_isUserActionsEnabled && m_mapDataSource && m_mapDataSource->canZoomUp()) {
        m_updateTimer->stop();
        m_isUserActionsEnabled = false;
        m_isLeftMousePressed = false;

        QPixmap pixmap(size());
        QPainter p;
        p.begin(&pixmap);
        m_slippyMapRenderer->render(&p, rect());
        p.end();
        ZoomUpAnimationRenderer *upAnimator = new ZoomUpAnimationRenderer(this, this);
        connect(upAnimator, SIGNAL(needUpdate()),
                this, SLOT(update()));
        upAnimator->setStartRect(rect());
        upAnimator->setEndRect(QRect(QPoint(centerTo.x() - centerTo.x() / 2, centerTo.y() - centerTo.y() / 2),
                                     size() / 2));
        upAnimator->setPixmap(pixmap);
        setRenderer(upAnimator);
        upAnimator->start();
        m_mapDataSource->adjustMap(rect(), m_mapDataSource->zoomLvl() + 1, centerTo);
    }
}

void SlippyMapWidget::zoomDown(const QPoint &centerTo)
{
    if (m_isUserActionsEnabled && m_mapDataSource && m_mapDataSource->canZoomDown()) {
        m_updateTimer->stop();
        m_isUserActionsEnabled = false;
        m_isLeftMousePressed = false;
        QPixmap pixmapFrom(size());
        QPainter p;
        p.begin(&pixmapFrom);
        m_slippyMapRenderer->render(&p, rect());
        p.end();

        m_mapDataSource->adjustMap(rect(), m_mapDataSource->zoomLvl() -1, centerTo);
        QPixmap pixmapTo(size());
        p.begin(&pixmapTo);
        m_slippyMapRenderer->render(&p, rect());
        p.end();
        ZoomDownAnimationRenderer *downAnimator = new ZoomDownAnimationRenderer(this, this);
        connect(downAnimator, SIGNAL(needUpdate()),
                this, SLOT(update()));
        downAnimator->setStartRect(rect());
        downAnimator->setEndRect(QRect(QPoint(centerTo.x() - centerTo.x() / 2, centerTo.y() - centerTo.y() / 2),
                                     size() / 2));
        downAnimator->setPixmap(pixmapFrom);
        downAnimator->setPixmapTo(pixmapTo);
        setRenderer(downAnimator);
        downAnimator->start();
    }
}

void SlippyMapWidget::centerMap()
{

}

void SlippyMapWidget::setRenderer(IRenderer *renderer)
{
    if (renderer) {
        m_currentRenderer = renderer;
    } else {
        m_currentRenderer = m_mainBatchRenderer;
        m_isUserActionsEnabled = true;
        m_isLeftMousePressed = false;
        m_updateTimer->start();
    }
}

void SlippyMapWidget::paintEvent(QPaintEvent *event)
{
    static int fps = -1;
    static int frameCount = 0;
    static quint64 lastTime = 0;
    if (lastTime == 0) {
        lastTime = QDateTime::currentMSecsSinceEpoch();
    }
    frameCount++;
    int dt = QDateTime::currentMSecsSinceEpoch() - lastTime;
    if (dt > 1000) {
        fps = (frameCount * 1000) / dt;
        frameCount = 0;
        lastTime = QDateTime::currentMSecsSinceEpoch();
    }
    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();

    bool timerIsActive = m_updateTimer->isActive();
    if (timerIsActive) {
        m_updateTimer->stop();
    }
    QTime t;
    t.start();
    if (m_currentRenderer) {
        QPainter p(this);
        p.setRenderHint(QPainter::NonCosmeticDefaultPen);
        m_currentRenderer->render(&p, event->rect());
        p.resetTransform();
        p.setPen(Qt::black);
        p.drawText(rect(),  Qt::AlignTop | Qt::TextWordWrap | Qt::AlignRight,
                    tr("fps: %1").arg(fps));
        p.drawText(rect(),  Qt::AlignBottom | Qt::TextWordWrap,
                    "Map data CCBYSA 2009 OpenStreetMap.org contributors");
#if 0
        QRect r = event->rect();
        r.translate(1, 1);
        r.setSize(r.size() - QSize(1, 1));
        p.setPen(QPen(Qt::black, 1));
        p.drawRect(r);
#endif
    }
    if (timerIsActive) {
        int delta = timerInterval() - (QDateTime::currentMSecsSinceEpoch() - m_lastUpdateTime);
        if (delta <= 0) {
            m_updateTimer->start(1);
        } else {
            m_updateTimer->start(delta);
        }
    }

}

void SlippyMapWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void SlippyMapWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if (m_isLeftMousePressed) {
        handleMouseMoveWithLeftButtonPressed();
    }
}

void SlippyMapWidget::mousePressEvent(QMouseEvent *event)
{

    if (m_mapDataSource && event->button() == Qt::LeftButton && m_isUserActionsEnabled){
        m_isLeftMousePressed = true;
        m_mousePressCoord = QCursor::pos();
        m_mapDeltaOnMousePress = m_mapDataSource->currentMapOffset();
    }
}

void SlippyMapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton){
        m_isLeftMousePressed = false;
    }
}

void SlippyMapWidget::wheelEvent(QWheelEvent *event)
{
    QWidget::wheelEvent(event);
    if (event->delta() > 0) {
        zoomUp(mapFromGlobal(QCursor::pos()));
    } else if (event->delta() < 0) {
        zoomDown(mapFromGlobal(QCursor::pos()));
    }
}

int SlippyMapWidget::timerInterval() const
{
    int rate = m_isLeftMousePressed ? m_maxFrameRateWhenMoveAnimation : m_maxFrameRate;
    return 1000 / rate;
}

void SlippyMapWidget::handleMouseMoveWithLeftButtonPressed()
{
    if (m_mapDataSource){
        QPoint newOffset = m_mapDeltaOnMousePress + m_mousePressCoord - QCursor::pos();
        m_mapDataSource->setCurrentMapOffset(newOffset);
    }
}
