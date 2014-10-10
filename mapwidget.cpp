#include "mapwidget.h"

#include "imapdata.h"

#include <QResizeEvent>
#include <QPainter>
#include <QDebug>
#include <QPainter>
#include <QFuture>
#include <QtConcurrentRun>
#include <QWheelEvent>
#include <QTimer>
#include <QPropertyAnimation>

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent),
    m_mapDataSource(nullptr),
    m_mapDelta(0, 0),
    m_isMousePressed(false),
    m_createBackgroundInProcess(false),
    m_isNeededUpdateAfterCreateBackground(true),
    m_isNeededToRecreatebackground(false),
    m_isZoomingEnabled(true)
{

}

void MapWidget::setMapData(IMapData * const mapDataSource, bool update)
{
    if (m_mapDataSource) {
        delete m_mapDataSource;
    }
    m_mapDataSource = mapDataSource;
    if (update){
        setMapDelta(QPoint(0, 0));
    }
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.drawImage(0, 0, m_backgroundImage);
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << event << event->size() << m_mapDataSource;
    QWidget::resizeEvent(event);
    setMapDelta(m_mapDelta);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if (m_isMousePressed){
        qDebug() << Q_FUNC_INFO << event->pos();
        QPoint delta = event->pos() - m_pressedMousePoint;
        setMapDelta(m_pressedMouseDelta - delta);
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton){
        m_isMousePressed = true;
        m_pressedMousePoint = event->pos();
        m_pressedMouseDelta = m_mapDelta;
    } else if (event->button() == Qt::RightButton) {
        QPointF coord = getCurrentGeoCoordByMousePos(event->pos());
        qDebug() << "current geo coord:" << coord.y() << "," << coord.x() << event->pos();

    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    m_isMousePressed = false;
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    QWidget::wheelEvent(event);
    if (m_mapDataSource && m_isZoomingEnabled){
        bool ok = true;
        if (event->delta() < 0){
            m_centerTo = getCurrentGeoCoordByMousePos(QPoint(size().width(), size().height()) / 2);
            m_lastBackgroundImage = m_backgroundImage;
            QPropertyAnimation *va = new QPropertyAnimation(this, "scaleForDownAnimation", this);
            connect(va, SIGNAL(valueChanged(QVariant)),
                    this, SLOT(valueChanged(QVariant)));
            connect(va, SIGNAL(finished()),
                    this, SLOT(onDownAnimationFinished()));
            va->setDuration(500);
            va->setStartValue(1.);
            va->setEndValue(.5);
            va->setEasingCurve(QEasingCurve::InQuad);
            va->start();

            ok = m_mapDataSource->zoomDown();
        } else if (event->delta() > 0) {
            m_centerTo = getCurrentGeoCoordByMousePos(event->pos());
            ok = m_mapDataSource->zoomUp();
            QTimer::singleShot(100, this, SLOT(enableZooming()));
        } else {
            ok = false;
        }
        if (ok) {
            centerByGeoCoord(m_centerTo.y(), m_centerTo.x());
            m_isNeededToRecreatebackground = true;
            m_isZoomingEnabled = false;
            m_isMousePressed = false;
        }
    }
}

void MapWidget::onDownAnimationFinished()
{
    qDebug() << Q_FUNC_INFO;
    if (sender()) {
        sender()->deleteLater();
    }
    m_isZoomingEnabled = true;
    if (m_mapDataSource) {
        m_mapDataSource->zoomDown();
       centerByGeoCoord(m_centerTo.y(), m_centerTo.x());
    }
}

void MapWidget::createBackground()
{
    m_backgroundImage = createBackground(size());
}

QImage MapWidget::createBackground(const QSize &size)
{
    if (size.isValid()) {
        QImage retImage(size, QImage::Format_RGB32);
        if (m_mapDataSource) {
            QPainter p(&retImage);
            int deltaXInTile = m_mapDelta.x() % m_mapDataSource->tileWidth();
            int deltaYInTile = m_mapDelta.y() % m_mapDataSource->tileHeight();
            int tileX = m_mapDelta.x() / m_mapDataSource->tileWidth();
            int tileY = m_mapDelta.y() / m_mapDataSource->tileHeight();
            qDebug() << Q_FUNC_INFO << tileX << tileY;
            int numOfXTiles = size.width() / m_mapDataSource->tileWidth() + (deltaXInTile ? 2 : 1);
            int numOfYTiles = size.height() / m_mapDataSource->tileHeight() + (deltaYInTile ? 2 : 1);
            int startX = deltaXInTile < 0 ? -1 : 0;
            int startY = deltaYInTile < 0 ? -1 : 0;

            for (int x = startX; x < numOfXTiles; ++x) {
                for (int y = startY; y < numOfYTiles; ++y) {
                    int realX = x * m_mapDataSource->tileWidth();
                    int realY = y * m_mapDataSource->tileHeight();
                    p.drawImage(realX - deltaXInTile, realY - deltaYInTile, m_mapDataSource->getTile(x + tileX, y + tileY));
                }
            }
            return retImage;
        }
    }
    return QImage();
}

QPointF MapWidget::getCurrentGeoCoordByMousePos(const QPoint &pos) const
{


    int currentPosInMapX =  m_mapDelta.x() + pos.x();
    int currentPosInMapY =  m_mapDelta.y() + pos.y();

    int currentXTile = currentPosInMapX / 0x0100;
    int currentXTileOffset = currentPosInMapX % 0x0100;

    int currentYTile = currentPosInMapY / 0x0100;
    int currentYTileOffset = currentPosInMapY % 0x0100;

    qreal lat = m_mapDataSource->tileY2Lat(currentYTile, currentYTileOffset);
    qreal lon = m_mapDataSource->tileX2Long(currentXTile, currentXTileOffset);

    qDebug() << Q_FUNC_INFO << pos << lon << lat;
    return QPointF(lon, lat);
}

void MapWidget::onCreateBackgroundFinished()
{
    m_createBackgroundInProcess = false;
    if (m_futureWatcherForFinishCreateBackground.isFinished()) {
        m_backgroundImage = m_futureWatcherForFinishCreateBackground.result();
        if (m_isNeededUpdateAfterCreateBackground){
            update();
        }
    }
}

void MapWidget::enableZooming()
{
    m_isZoomingEnabled = true;
}

void MapWidget::valueChanged(QVariant value)
{
    qDebug() << Q_FUNC_INFO;
    QTransform trans;
    double scale = value.toDouble();
    trans.scale(scale, scale);
    QImage temp = m_lastBackgroundImage.transformed(trans);
    m_backgroundImage.fill(Qt::gray);
    int deltaX = (m_backgroundImage.width() - temp.width()) / 2;
    int deltaY = (m_backgroundImage.height() - temp.height()) / 2;
    QPainter p(&m_backgroundImage);
    p.drawImage(deltaX, deltaY, temp);
    update();
}

void MapWidget::createBackgroundInThreadAndUpdate(bool update)
{

    m_isNeededUpdateAfterCreateBackground = update;
    if (!m_createBackgroundInProcess /* || m_isNeededToRecreatebackground */) {
        m_createBackgroundInProcess = true;
        m_isNeededToRecreatebackground = false;
        QFuture<QImage> f = QtConcurrent::run(this, &MapWidget::createBackground, size());
        connect(&m_futureWatcherForFinishCreateBackground, SIGNAL(finished()), this, SLOT(onCreateBackgroundFinished()));
        m_futureWatcherForFinishCreateBackground.setFuture(f);
    }
}

int MapWidget::mapDeltaX() const
{
    return m_mapDelta.x();
}

int MapWidget::mapDeltaY() const
{
    return m_mapDelta.y();
}

void MapWidget::setMapDelta(QPoint mapDelta)
{
    if (!m_mapDataSource) {
        return;
    }

    m_mapDelta = mapDelta;
    QPoint maxDelta(m_mapDataSource->mapSizeInPx().width() - size().width(),
                   m_mapDataSource->mapSizeInPx().height() - size().height());
    if (maxDelta.x() < 0 ) {
        m_mapDelta.setX(maxDelta.x() / 2);
    } else if (mapDelta.x() > maxDelta.x()) {
        m_mapDelta.setX(maxDelta.x());
    }

    if (maxDelta.y() < 0 ) {
        m_mapDelta.setY(maxDelta.y() / 2);
    } else if (mapDelta.y() > maxDelta.y()) {
        m_mapDelta.setY(maxDelta.y());
    }

    createBackgroundInThreadAndUpdate(true);
}

void MapWidget::centerByGeoCoord(const double &lat, const double &lon)
{
    if (m_mapDataSource){
        int tileX, tileXOffset;
        int tileY, tileYOffset;
        tileX = m_mapDataSource->long2TileX(lon, &tileXOffset);
        tileY = m_mapDataSource->lat2TileY(lat, &tileYOffset);
        int x = tileX * m_mapDataSource->tileWidth() + tileXOffset;
        int y = tileY * m_mapDataSource->tileHeight() + tileYOffset;
        x -= size().width() / 2;
        y -= size().height() / 2;
        if (x < 0) x = 0;
        if (y < 0) y = 0;

        setMapDelta(QPoint(x, y));
    }
}

double MapWidget::scaleForDownAnimation() const
{
    return 0.;
}

void MapWidget::setScaleForDownAnimation(const double &scale)
{
    qDebug() << Q_FUNC_INFO << scale;
}


