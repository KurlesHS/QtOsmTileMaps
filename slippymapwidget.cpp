#include "slippymapwidget.h"
#include "imapdata.h"
#include "slippymaprenderer.h"
#include "zoomdownanimationrenderer.h"
#include "zoomupanimationrenderer.h"

#include <QDebug>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>

SlippyMapWidget::SlippyMapWidget(QWidget *parent) :
    QGLWidget(parent),
    m_mapDataSource(nullptr),
    m_isLeftMousePressed(false),
    m_isUserActionsEnabled(true),
    m_slippyMapRenderer(new SlippyMapRenderer(this)),
    m_currentRenderer(m_slippyMapRenderer)

{
    setAutoFillBackground(false);
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
        m_isUserActionsEnabled = false;
        m_isLeftMousePressed = false;

        QPixmap pixmap(size());
        QPainter p;
        p.begin(&pixmap);
        m_slippyMapRenderer->render(&p, rect());
        p.end();
        ZoomDownAnimationRenderer *downAnimator = new ZoomDownAnimationRenderer(this, this);
        connect(downAnimator, SIGNAL(needUpdate()),
                this, SLOT(update()));
        downAnimator->setStartRect(rect());
        downAnimator->setEndRect(QRect(QPoint(centerTo.x() - centerTo.x() / 2, centerTo.y() - centerTo.y() / 2),
                                     size() / 2));
        downAnimator->setPixmap(pixmap);
        setRenderer(downAnimator);
        downAnimator->start();
        m_mapDataSource->adjustMap(rect(), m_mapDataSource->zoomLvl() -1, centerTo);
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
        m_currentRenderer = m_slippyMapRenderer;
        m_isUserActionsEnabled = true;
        m_isLeftMousePressed = false;
    }
}

void SlippyMapWidget::paintEvent(QPaintEvent *event)
{
    if (m_currentRenderer) {
        QPainter p;
        p.begin(this);
        p.setRenderHint(QPainter::Antialiasing);
        m_currentRenderer->render(&p, event->rect());
        p.resetTransform();
        QRect r = event->rect();
        r.translate(1, 1);
        r.setSize(r.size() - QSize(1, 1));
        p.setPen(QPen(Qt::black, 1));
        p.drawRect(r);
        p.end();
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

void SlippyMapWidget::handleMouseMoveWithLeftButtonPressed()
{
    if (m_mapDataSource){
        QPoint newOffset = m_mapDeltaOnMousePress + m_mousePressCoord - QCursor::pos();
        m_mapDataSource->setCurrentMapOffset(newOffset);
        update();
    }
}
