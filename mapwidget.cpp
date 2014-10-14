#include "mapwidget.h"

#include "imapdata.h"
#include "maprenderer.h"

#include <QResizeEvent>
#include <QPainter>
#include <QDebug>
#include <QPainter>
#include <QFuture>
#include <QtConcurrentRun>
#include <QWheelEvent>
#include <QTimer>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent),
    m_mapRenderer(new MapRenderer()),
    m_threadForRenderer(new QThread(this)),
    m_timerForResizeEvent(new QTimer(this)),
    m_isLeftMousePressed(false)
{
    m_mapRenderer->moveToThread(m_threadForRenderer);
    m_threadForRenderer->start();
    connect(m_mapRenderer, SIGNAL(updateImage(QImage)),
            this, SLOT(updatedImage(QImage)));
    connect(m_timerForResizeEvent, SIGNAL(timeout()),
            this, SLOT(onResizeTimeout()));
}

MapWidget::~MapWidget()
{
    m_threadForRenderer->quit();
    m_threadForRenderer->wait(2000);
}

void MapWidget::setMapData(IMapData * const mapDataSource)
{
    mapDataSource->moveToThread(m_threadForRenderer);
    QMetaObject::invokeMethod(m_mapRenderer, "setMapData",
                              Qt::QueuedConnection, Q_ARG(QObject*, mapDataSource));
}

void MapWidget::zoomUp(const QPoint &centerTo)
{
    QMetaObject::invokeMethod(m_mapRenderer, "zoomUp", Qt::QueuedConnection,
                              Q_ARG(QPoint, centerTo));
}

void MapWidget::zoomDown(const QPoint &centerTo)
{
    QMetaObject::invokeMethod(m_mapRenderer, "zoomDown", Qt::QueuedConnection,
                              Q_ARG(QPoint, centerTo));
}

void MapWidget::centerMap()
{
    QMetaObject::invokeMethod(m_mapRenderer, "centerMap", Qt::QueuedConnection);
}

void MapWidget::updatedImage(const QImage &image)
{
    m_background = image;
    update();
}

void MapWidget::onResizeTimeout()
{
    m_timerForResizeEvent->stop();
    QMetaObject::invokeMethod(m_mapRenderer, "setSize", Qt::QueuedConnection,
                              Q_ARG(QSize, size()));
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (size().isValid()  && m_background.size().isValid()) {
        QPainter p(this);
        p.drawImage(0, 0, m_background);
        p.drawRect(0, 0, size().width() - 1, size().height() - 1);
    }
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    //m_background = QImage(event->size(), QImage::Format_ARGB32);
    //m_background.fill(Qt::gray);
     m_timerForResizeEvent->start(100);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if (m_isLeftMousePressed) {
        handleMouseMoveWithLeftButtonPressed();
    }

}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        m_isLeftMousePressed = true;
        m_mousePressCoord = QCursor::pos();
        m_mapDeltaOnMousePress = m_mapRenderer->mapDelta();
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton){
        m_isLeftMousePressed = false;
    }
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    QWidget::wheelEvent(event);
    if (event->delta() > 0) {
        zoomUp(mapFromGlobal(QCursor::pos()));
    } else if (event->delta() < 0) {
        zoomDown(mapFromGlobal(QCursor::pos()));
    }
}

void MapWidget::handleMouseMoveWithLeftButtonPressed()
{
    QPoint delta = QCursor::pos() - m_mousePressCoord;
    QPoint newMapDelta = m_mapDeltaOnMousePress - delta;
    if (m_mapRenderer){
        m_mapRenderer->setMapDelta(newMapDelta);
    }
}



