#include "maprenderer.h"
#include "imapdata.h"
#include "mapscene.h"
#include "geocoord.h"

#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QTimer>
#include <QTime>

#define ANIMATION_TIME 300

QImage fadeImages(const QImage &src1, const QImage &src2, unsigned char fadeval, unsigned char fadeMax)
{
    QImage dest(src1.size(), src1.format());
    const double e1 = (double)fadeval/fadeMax;
    const double e2 = (fadeMax-fadeval)/(double)fadeMax;
    for(int y = 0; y < dest.height(); y++)
    {
        QRgb *destrow = (QRgb*)dest.scanLine(y);
        QRgb *src1row = (QRgb*)src1.scanLine(y);
        QRgb *src2row = (QRgb*)src2.scanLine(y);
        for(int x = 0; x < dest.width(); x++)
        {
            ((unsigned char*)&destrow[x])[0] = (((unsigned char*)&src1row[x])[0]*e1 + ((unsigned char*)&src2row[x])[0]*e2);
            ((unsigned char*)&destrow[x])[1] = (((unsigned char*)&src1row[x])[1]*e1 + ((unsigned char*)&src2row[x])[1]*e2);
            ((unsigned char*)&destrow[x])[2] = (((unsigned char*)&src1row[x])[2]*e1 + ((unsigned char*)&src2row[x])[2]*e2);
            ((unsigned char*)&destrow[x])[3] = 255;
        }
    }
    return dest;
}

MapRenderer::MapRenderer(QObject *parent) :
    QObject(parent),
    m_mapData(nullptr),
    m_mapScene(new MapScene(this)),
    m_isBusy(false),
    m_isZoomEnabled(true)
{

}

void MapRenderer::setMapData(QObject *mapData)
{
    if (m_mapData) {
        delete m_mapData;
    }
    m_mapData = qobject_cast<IMapData*>(mapData);
    if (m_mapData){
        m_mapScene->setMaxX(m_mapData->maxX());
        m_mapScene->setMaxY(m_mapData->maxY());
        m_mapScene->setMinX(m_mapData->minX());
        m_mapScene->setMinY(m_mapData->minY());
        m_mapScene->setZoomLevel(m_mapData->zoomLvl());
        QSize mapSizeInPx = m_mapData->mapSizeInPx();
        QPoint mapDelta(0, 0);
        if (mapSizeInPx.width() < m_size.width()) {
            mapDelta.setX((mapSizeInPx.width() - m_size.width()) / 2);
        }
        if (mapSizeInPx.height() < m_size.height()) {
            mapDelta.setY((mapSizeInPx.height() - m_size.height()) / 2);
        }
        m_mapScene->setMapDelta(mapDelta);
    }
}

void MapRenderer::update()
{
    if (!m_size.isValid() || !m_mapData) {
        m_isBusy = false;
        return;
    }
    {
        QPainter p(&m_background);
        m_mapScene->render(p, QRect(QPoint(0, 0), m_size), m_mapData);
    }
    updateImage(m_background);
    m_isBusy = false;
}

void MapRenderer::setMapDelta(const QPoint &pos, bool update)
{
    if (!m_isBusy || !m_isZoomEnabled) {
        m_isBusy = true;
        m_mapScene->setMapDelta(pos);
        if (update) {
            QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
        }
    }
}

QPoint MapRenderer::mapDelta() const
{
    QPoint delta;
    if (m_mapScene) {
        delta = m_mapScene->mapDelta();
    }
    return delta;
}

void MapRenderer::setSize(const QSize &size)
{
    m_size = size;
    m_background = QImage(size, QImage::Format_ARGB32_Premultiplied);
    m_background.fill(Qt::gray);
    update();
}

void MapRenderer::centerMap()
{
    if (m_mapData){
        QPointF centerTiles(0, 0);
        centerTiles += QPoint((m_mapData->maxX() - m_mapData->minX()) / 2. + m_mapData->minX(),
                              (m_mapData->maxY() - m_mapData->minY()) / 2. + m_mapData->minY());

    }
}

void MapRenderer::updateBackground()
{
    if (m_mapData){
        QPainter p(&m_background);
        m_mapScene->render(p, QRect(QPoint(0, 0), m_size), m_mapData);
    }
}

void MapRenderer::enableZoom()
{
    m_isZoomEnabled = true;
}

void MapRenderer::handleZoomUpState(const ZoomAnimationHelper &zah)
{
    m_background = m_imageForZoom.copy(zah.rect).scaled(m_size);
    emit updateImage(m_background);

}

void MapRenderer::handleZoomDownState(const ZoomAnimationHelper &zah)
{
    QSize scaleTo = zah.rect.size();
    QImage currentImg = m_imageForZoom.scaled(scaleTo);
    QImage tmpImage(m_size, QImage::Format_ARGB32_Premultiplied);
    tmpImage.fill(Qt::gray);
    m_background.fill(Qt::gray);
    QPainter p (&m_background);
    p.drawImage(zah.rect, currentImg);
    emit updateImage(m_background);
}

void MapRenderer::handleFadeAnimationState(const ZoomAnimationHelper &zah, const ZoomAnimationHelper &zahEndValue)
{
    int max = zahEndValue.scaleFactor;
    int current = zah.scaleFactor;
    emit updateImage(fadeImages(m_background, m_imageForZoom, current, max));
}

void MapRenderer::startFadeAnimation()
{
    m_imageForZoom = m_background;
    updateBackground();
    m_currentState = FadeAnimationState;
    ZoomAnimationHelper from, to;
    from.scaleFactor = 0;
    to.scaleFactor = 5;
    ZoomAnimation *za = new ZoomAnimation(this);
    za->setStartValue(QVariant::fromValue<ZoomAnimationHelper>(from));
    za->setEndValue(QVariant::fromValue<ZoomAnimationHelper>(to));
    za->setDuration(ANIMATION_TIME);
    connectZoomAnimation(za);
    za->start();

}

void MapRenderer::onAnimation(const QVariant &value)
{
    ZoomAnimation *za = qobject_cast<ZoomAnimation*>(sender());
    ZoomAnimationHelper zahEndValue = za->endValue().value<ZoomAnimationHelper>();
    ZoomAnimationHelper zah = value.value<ZoomAnimationHelper>();
    switch (m_currentState) {
    case ZoomUpAnimationState:
        handleZoomUpState(zah);
        break;
    case ZoomDownAnimationState:
        handleZoomDownState(zah);
        break;
    case FadeAnimationState:
        handleFadeAnimationState(zah, zahEndValue);
        break;
    default:
        break;
    }
}

void MapRenderer::onAnimationFinished()
{
    QObject *sender = this->sender();
    if (sender) {
        sender->deleteLater();
    }
    switch (m_currentState) {
    case ZoomDownAnimationState:
    case ZoomUpAnimationState:
        startFadeAnimation();
        break;
    default:
        m_currentState = NormalState;
        m_isZoomEnabled = true;
        break;
    }
}

void MapRenderer::connectZoomAnimation(ZoomAnimation *za)
{
    connect(za, SIGNAL(finished()), this, SLOT(onAnimationFinished()));
    connect(za, SIGNAL(valueChanged(QVariant)), this, SLOT(onAnimation(QVariant)));
}

void MapRenderer::zoomUp(const QPoint &centerTo)
{
    if (m_mapData && m_mapData->canZoomUp() && m_isZoomEnabled) {
        m_isZoomEnabled = false;
        qreal tileX = (mapDelta().x() + centerTo.x()) / (qreal)m_mapData->tileWidth();
        qreal tileY = (mapDelta().y() + centerTo.y()) / (qreal)m_mapData->tileHeight();

        GeoCoord centerCoord = GeoCoord::fromTilePosition(
                    QPointF(tileX + m_mapData->minX(), tileY + m_mapData->minY()),
                    m_mapData->zoomLvl());
        m_mapData->zoomUp();
        QSize newSize = m_size / 2;
        QPointF newCenter = centerCoord.getTilePosition(m_mapData->zoomLvl());
        newCenter -= QPoint(m_mapData->minX(), m_mapData->minY());
        newCenter *= m_mapData->tileHeight();
        QPointF newDelta = newCenter - centerTo;
        m_mapScene->setMapDelta(QPoint(newDelta.x(), newDelta.y()));

        QRect endRect(QPoint(centerTo.x() - centerTo.x() / 2, centerTo.y() - centerTo.y() / 2),
                         newSize);
        QRect startRect(QPoint(0, 0), m_size);
        m_imageForZoom = m_background;
        ZoomAnimation *za = new ZoomAnimation(this);
        connectZoomAnimation(za);
        ZoomAnimationHelper from, to;
        from.rect = startRect;
        to.rect = endRect;
        m_currentState = ZoomUpAnimationState;
        za->setStartValue(QVariant::fromValue<ZoomAnimationHelper>(from));
        za->setEndValue(QVariant::fromValue<ZoomAnimationHelper>(to));
        za->setDuration(ANIMATION_TIME);
        za->start();
    }
}

void MapRenderer::zoomDown(const QPoint &centerTo)
{
    if (m_mapData && m_mapData->canZoomDown() && m_isZoomEnabled) {
        m_isZoomEnabled = false;
        QSize newSize = m_size / 2;
        qreal tileX = (mapDelta().x() + centerTo.x()) / (qreal)m_mapData->tileWidth();
        qreal tileY = (mapDelta().y() + centerTo.y()) / (qreal)m_mapData->tileHeight();

        GeoCoord centerCoord = GeoCoord::fromTilePosition(
                    QPointF(tileX + m_mapData->minX(), tileY + m_mapData->minY()),
                    m_mapData->zoomLvl());
        m_mapData->zoomDown();
        QPointF newCenter = centerCoord.getTilePosition(m_mapData->zoomLvl());
        newCenter -= QPoint(m_mapData->minX(), m_mapData->minY());
        newCenter *= m_mapData->tileHeight();
        QPointF newDelta = newCenter - centerTo;
        m_mapScene->setMapDelta(QPoint(newDelta.x(), newDelta.y()));
        QRect endRect(QPoint(centerTo.x() - centerTo.x() / 2, centerTo.y() - centerTo.y() / 2),
                      newSize);
        QRect startRect(QPoint(0, 0), m_size);
        m_imageForZoom = m_background;
        ZoomAnimation *za = new ZoomAnimation(this);
        connectZoomAnimation(za);
        ZoomAnimationHelper from, to;
        from.rect = startRect;
        to.rect = endRect;
        m_currentState = ZoomDownAnimationState;
        za->setStartValue(QVariant::fromValue<ZoomAnimationHelper>(from));
        za->setEndValue(QVariant::fromValue<ZoomAnimationHelper>(to));
        za->setDuration(ANIMATION_TIME);
        za->start();
    }
}

