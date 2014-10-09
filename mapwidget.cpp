#include "mapwidget.h"

#include "imapdata.h"

#include <QResizeEvent>
#include <QPainter>
#include <QDebug>
#include <QPainter>
#include <QFuture>
#include <QtConcurrentRun>
MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent),
    m_mapDataSource(nullptr),
    m_mapDeltaX(0),
    m_mapDeltaY(0),
    m_isMousePressed(false),
    m_createBackgroundInProcess(false),
    m_isNeededUpdateAfterCreateBackground(true)
{
    setMouseTracking(true);
}

void MapWidget::setMapData(IMapData * const mapDataSource)
{
    if (m_mapDataSource) {
        delete m_mapDataSource;
    }
    m_mapDataSource = mapDataSource;
    createBackgroundInThreadAndUpdate(true);
    //update();
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.drawImage(0, 0, m_backgroundImage);
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    //createBackground();
    createBackgroundInThreadAndUpdate(true);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if (m_isMousePressed){
        qDebug() << Q_FUNC_INFO << event->pos();
        QPoint delta = event->pos() - m_pressedMousePoint;
        setMapDelta(QPoint(m_pressedMouseDeltaX - delta.x(),
                           m_pressedMouseDeltaY - delta.y()));
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton){
        m_isMousePressed = true;
        m_pressedMousePoint = event->pos();
        m_pressedMouseDeltaX = m_mapDeltaX;
        m_pressedMouseDeltaY = m_mapDeltaY;
    } else if (event->button() == Qt::RightButton) {
        int currentPosInMapX =  m_mapDeltaX + event->pos().x();
        int currentPosInMapY =  m_mapDeltaY + event->pos().y();

        int currentXTile = currentPosInMapX / 0x0100;
        int currentXTileOffset = currentPosInMapX % 0x0100;

        int currentYTile = currentPosInMapY / 0x0100;
        int currentYTileOffset = currentPosInMapY % 0x0100;

        qDebug() << "current geo coord: " << m_mapDataSource->tileY2Lat(currentYTile, currentYTileOffset)
                 << "," << m_mapDataSource->tileX2Long(currentXTile, currentXTileOffset);
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    m_isMousePressed = false;
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
            int deltaXInTile = m_mapDeltaX % m_mapDataSource->tileWidth();
            int deltaYInTile = m_mapDeltaY % m_mapDataSource->tileHeight();
            int tileX = m_mapDeltaX / m_mapDataSource->tileWidth();
            int tileY = m_mapDeltaY / m_mapDataSource->tileHeight();
            qDebug() << Q_FUNC_INFO << tileX << tileY;
            int numOfXTiles = size.width() / m_mapDataSource->tileWidth() + (deltaXInTile ? 2 : 1);
            int numOfYTiles = size.height() / m_mapDataSource->tileHeight() + (deltaYInTile ? 2 : 1);
            for (int x = 0; x < numOfXTiles; ++x) {
                for (int y = 0; y < numOfYTiles; ++y) {
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

void MapWidget::onCreateBackgroundFinished()
{
    if (m_futureWatcherForFinishCreateBackground.isFinished()) {
        m_backgroundImage = m_futureWatcherForFinishCreateBackground.result();
        if (m_isNeededUpdateAfterCreateBackground){
            update();
        }
    }
    m_createBackgroundInProcess = false;

}

void MapWidget::createBackgroundInThreadAndUpdate(bool update)
{
    m_isNeededUpdateAfterCreateBackground = update;
    if (!m_createBackgroundInProcess) {
        m_createBackgroundInProcess = true;
        QFuture<QImage> f = QtConcurrent::run(this, &MapWidget::createBackground, size());
        m_futureWatcherForFinishCreateBackground.setFuture(f);
        connect(&m_futureWatcherForFinishCreateBackground, SIGNAL(finished()), this, SLOT(onCreateBackgroundFinished()));
    }

}

int MapWidget::mapDeltaX() const
{
    return m_mapDeltaX;
}

int MapWidget::mapDeltaY() const
{
    return m_mapDeltaY;
}

void MapWidget::setMapDeltaX(int mapDeltaX)
{
    setMapDeltaXHelper(mapDeltaX);
    createBackgroundInThreadAndUpdate(true);
    //createBackground();
    //update();
}

void MapWidget::setMapDeltaY(int mapDeltaY)
{
    setMapDeltaYHelper(mapDeltaY);
    createBackgroundInThreadAndUpdate(true);
    //createBackground();
    //update();
}

void MapWidget::setMapDeltaXHelper(int mapDeltaX)
{
    if (!m_mapDataSource) {
        return;
    }
    m_mapDeltaX = mapDeltaX > 0 ? mapDeltaX : 0;
    int maxDeltaX = m_mapDataSource->mapSizeInPx().width() - size().width();
    if (m_mapDeltaX > maxDeltaX) {
        m_mapDeltaX = maxDeltaX;
    }
    qDebug() << Q_FUNC_INFO << maxDeltaX << m_mapDeltaX;
}

void MapWidget::setMapDeltaYHelper(int mapDeltaY)
{
    m_mapDeltaY = mapDeltaY > 0 ? mapDeltaY : 0;
    int maxDeltaY = m_mapDataSource->mapSizeInPx().height() - size().height();
    if (m_mapDeltaY > maxDeltaY) {
        m_mapDeltaY = maxDeltaY;
    }
    qDebug() << Q_FUNC_INFO << maxDeltaY << m_mapDeltaY << size().height() << size().width();
}

void MapWidget::setMapDelta(QPoint mapDelta)
{
    setMapDeltaX(mapDelta.x());
    setMapDeltaY(mapDelta.y());
    createBackgroundInThreadAndUpdate(true);
    //createBackground();
    //update();
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
        setMapDelta(QPoint(x, y));
    }
}


