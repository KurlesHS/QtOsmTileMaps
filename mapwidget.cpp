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
#include <QDateTime>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

static const char *currentFadeAnimationTickProperty = "currentTick";
static const char *maxFadeAnimationTickProperty = "maxTick";
static const char *currentTimeProperty = "currentTime";

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

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent),
    m_currentState(NormalState),
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
    QWidget::resizeEvent(event);
    setMapDelta(m_mapDelta);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if (m_isMousePressed){
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
        QPointF coord = getCurrentGeoCoordByPos(event->pos());
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
            int currentZoomLevel = m_mapDataSource->zoomLvl();
            if (currentZoomLevel <= m_mapDataSource->minZoomLevel()) {
                return;
            }
            m_centerTo = getCurrentGeoCoordByPos(QPoint(size().width(), size().height()) / 2);
            m_lastBackgroundImage = m_backgroundImage;
            //QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);
            //QPropertyAnimation *va = new QPropertyAnimation(this, "scaleForDownAnimation", this);
            //connect(animGroup, SIGNAL(currentLoopChanged(int)), this, SLOT(currentLoopParalelGroupAnimationChanged(int)));
            //animGroup->addAnimation(va);
            QPoint now, down;
            now = getPosInByGeoCoords(m_centerTo.y(), m_centerTo.x());
            QPoint mapDeltaNow = getMapDeltaByCenterGeoCoord(m_centerTo.y(), m_centerTo.x());
            m_mapDataSource->zoomDown();
            QPoint mapDeltaDown = getMapDeltaByCenterGeoCoord(m_centerTo.y(), m_centerTo.x());
            QPoint delta = (m_mapDelta / 2 - mapDeltaDown);
            down = getPosInByGeoCoords(m_centerTo.y(), m_centerTo.x());
            QSize mapDownSize = m_mapDataSource->mapSizeInPx();
            m_mapDataSource->zoomUp();
            ZoomAnimation *za = new ZoomAnimation(this);
            ZoomAnimationHelper from, to;
            QSize idealPos(delta.x(), delta.y());
            from.position = QSize(0, 0);
            //to.position = QSize((size() - size() * 0.5) / 2);
            //if (to.position != idealPos)
            to.position = idealPos;
            from.scaleFactor = 1.;
            to.scaleFactor = 0.5;
            if (mapDownSize.width() < size().width() || mapDownSize.height() < size().height()) {
                qDebug() << "fail!!!";
            }

            qDebug() << Q_FUNC_INFO  << delta << (size() - size() * 0.5) / 2 << m_mapDelta << mapDeltaNow << mapDeltaDown;
            qDebug() << Q_FUNC_INFO << (now / 2 - down);

            connect(za, SIGNAL(finished()),
                    this, SLOT(onDownAnimationFinished()));
            connect(za, SIGNAL(valueChanged(QVariant)),
                    this, SLOT(onZoomDownAnimationTick(QVariant)));
            za->setDuration(500);
            za->setStartValue(QVariant::fromValue<ZoomAnimationHelper>(from));
            za->setEndValue(QVariant::fromValue<ZoomAnimationHelper>(to));
            za->setEasingCurve(QEasingCurve::InQuad);
            za->start();
            ok = false;
            m_isNeededToRecreatebackground = true;
            m_isZoomingEnabled = false;
            m_isMousePressed = false;
        } else if (event->delta() > 0) {
            m_centerTo = getCurrentGeoCoordByPos(event->pos());
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
    if (sender()) {
        sender()->deleteLater();
    }
    if (m_mapDataSource) {
        m_mapDataSource->zoomDown();
        m_currentState = FadeDownState;
        centerByGeoCoord(m_centerTo.y(), m_centerTo.x());
    }
}

void MapWidget::onFadeAnimation()
{
    QTimer *t = qobject_cast<QTimer*>(sender());
    if (t) {
        t->stop();
        int currentTick = t->property(currentFadeAnimationTickProperty).toInt();
        int maxTick = t->property(maxFadeAnimationTickProperty).toInt();
        continueFadeDownAnimation(currentTick, maxTick, 0);
        t->deleteLater();
    }
}

void MapWidget::onFadeResult()
{
    QFutureWatcher<QImage> *fw = dynamic_cast<QFutureWatcher<QImage> *>(sender());
    if (fw) {
        quint64 startCreationTime = fw->property(currentTimeProperty).toULongLong();
        int currentTick = fw->property(currentFadeAnimationTickProperty).toInt();
        int maxTick = fw->property(maxFadeAnimationTickProperty).toInt();
        m_backgroundImage = fw->result();
        update();
        delete fw;
        if (currentTick == maxTick) {
            m_currentState = NormalState;
            m_isZoomingEnabled = true;
            return;
        }
        ++currentTick;
        int delta = 50 - (QDateTime::currentMSecsSinceEpoch() - startCreationTime);
        continueFadeDownAnimation(currentTick, maxTick, delta);
    }
}

void MapWidget::currentLoopParalelGroupAnimationChanged(const int loop)
{
    qDebug() << Q_FUNC_INFO << loop;
}

void MapWidget::onZoomDownAnimationTick(const QVariant &value)
{

    ZoomAnimationHelper za = value.value<ZoomAnimationHelper>();
    QTransform trans;
    trans.scale(za.scaleFactor, za.scaleFactor);
    QImage temp = m_lastBackgroundImage.transformed(trans);
    m_backgroundImage.fill(Qt::gray);
    //int deltaX = (m_backgroundImage.width() - temp.width()) / 2;
    //int deltaY = (m_backgroundImage.height() - temp.height()) / 2;
    QPainter p(&m_backgroundImage);
    p.drawImage(za.position.width(), za.position.height(), temp);
    update();
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

QPointF MapWidget::getCurrentGeoCoordByPos(const QPoint &pos) const
{
    int currentPosInMapX =  m_mapDelta.x() + pos.x();
    int currentPosInMapY =  m_mapDelta.y() + pos.y();

    int currentXTile = currentPosInMapX / 0x0100;
    int currentXTileOffset = currentPosInMapX % 0x0100;

    int currentYTile = currentPosInMapY / 0x0100;
    int currentYTileOffset = currentPosInMapY % 0x0100;

    qreal lat = m_mapDataSource->tileY2Lat(currentYTile, currentYTileOffset);
    qreal lon = m_mapDataSource->tileX2Long(currentXTile, currentXTileOffset);

    return QPointF(lon, lat);
}

void MapWidget::continueFadeDownAnimation(const int currentTick, const int maxTick, const int delta)
{
    if (delta <= 0) {
        QFutureWatcher<QImage> *fw = new QFutureWatcher<QImage>;
        connect(fw, SIGNAL(finished()), this, SLOT(onFadeResult()));
        QFuture<QImage> f = QtConcurrent::run(&fadeImages, m_fadeImage2, m_fadeImage1, currentTick, maxTick);
        fw->setFuture(f);
        fw->setProperty(currentFadeAnimationTickProperty, currentTick);
        fw->setProperty(maxFadeAnimationTickProperty, maxTick);
        fw->setProperty(currentTimeProperty, QDateTime::currentMSecsSinceEpoch());
    } else {
        auto t = new QTimer();
        connect(t, SIGNAL(timeout()), this, SLOT(onFadeAnimation()));
        t->setProperty(currentFadeAnimationTickProperty, currentTick);
        t->setProperty(maxFadeAnimationTickProperty, maxTick);
        t->setProperty(currentTimeProperty, QDateTime::currentMSecsSinceEpoch());
        t->start(delta);
    }
}

void MapWidget::onCreateBackgroundFinished()
{
    m_createBackgroundInProcess = false;
    if (m_futureWatcherForFinishCreateBackground.isFinished()) {
        switch (m_currentState) {
        case NormalState:
        {
            m_backgroundImage = m_futureWatcherForFinishCreateBackground.result();
            if (m_isNeededUpdateAfterCreateBackground){
                update();
            }
        }
            break;
        case FadeDownState: {
            m_currentState = SkipUpdateState;
            m_fadeImage2 = m_futureWatcherForFinishCreateBackground.result();
            m_fadeImage1 = m_backgroundImage;
            continueFadeDownAnimation(0, 5, 0);
        }
            break;
        case SkipUpdateState: {

        }
            break;
        }
    }
}

void MapWidget::enableZooming()
{
    m_isZoomingEnabled = true;
}

void MapWidget::createBackgroundInThreadAndUpdate(bool update)
{

    m_isNeededUpdateAfterCreateBackground = update;
    if (!m_createBackgroundInProcess /* || m_isNeededToRecreatebackground */) {
        m_createBackgroundInProcess = true;
        m_isNeededToRecreatebackground = false;
        QFuture<QImage> f = QtConcurrent::run(this, &MapWidget::createBackground, size());
        m_futureWatcherForFinishCreateBackground.disconnect();
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


void MapWidget::setMapDelta(QPoint mapDelta, const bool update)
{
    if (!m_mapDataSource) {
        return;
    }

    m_mapDelta = mapDelta;
    QPoint maxDelta(m_mapDataSource->mapSizeInPx().width() - size().width(),
                   m_mapDataSource->mapSizeInPx().height() - size().height());
    if (maxDelta.x() < 0) {
        m_mapDelta.setX(maxDelta.x() / 2);
    } else if (mapDelta.x() > maxDelta.x()) {
        m_mapDelta.setX(maxDelta.x());
    } else if (mapDelta.x() < 0) {
        m_mapDelta.setX(0);
    }

    if (maxDelta.y() < 0) {
        m_mapDelta.setY(maxDelta.y() / 2);
    } else if (mapDelta.y() > maxDelta.y()) {
        m_mapDelta.setY(maxDelta.y());
    } else if (mapDelta.y() < 0) {
        m_mapDelta.setY(0);
    }

    createBackgroundInThreadAndUpdate(update);
}

void MapWidget::centerByGeoCoord(const double &lat, const double &lon)
{
    if (m_mapDataSource){
        m_mapDelta = getMapDeltaByCenterGeoCoord(lat, lon);
        qDebug() << Q_FUNC_INFO << m_mapDelta;
        createBackgroundInThreadAndUpdate(true);
    }
}

QPoint MapWidget::getMapDeltaByCenterGeoCoord(const double &lat, const double &lon)
{
    QPoint mapDelta;
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
        mapDelta.setX(x);
        mapDelta.setY(y);

        QPoint maxDelta(m_mapDataSource->mapSizeInPx().width() - size().width(),
                       m_mapDataSource->mapSizeInPx().height() - size().height());

        if (maxDelta.x() < 0 ) {
            mapDelta.setX(maxDelta.x() / 2);
        } else if (mapDelta.x() > maxDelta.x()) {
            mapDelta.setX(maxDelta.x());
        }

        if (maxDelta.y() < 0 ) {
            mapDelta.setY(maxDelta.y() / 2);
        } else if (mapDelta.y() > maxDelta.y()) {
            mapDelta.setY(maxDelta.y());
        }
        qDebug() << Q_FUNC_INFO << tileXOffset << tileYOffset;
    }
    return mapDelta;
}

QPoint MapWidget::getPosInByGeoCoords(const double &lat, const double &lon)
{
    QPoint mapDelta;
    if (m_mapDataSource){
        int tileX, tileXOffset;
        int tileY, tileYOffset;
        tileX = m_mapDataSource->long2TileX(lon, &tileXOffset);
        tileY = m_mapDataSource->lat2TileY(lat, &tileYOffset);
        mapDelta.setX(tileX * m_mapDataSource->tileWidth() + tileXOffset);
        mapDelta.setY(tileY * m_mapDataSource->tileHeight() + tileYOffset);
    }
    return mapDelta;
}

double MapWidget::scaleForDownAnimation() const
{
    return 0.;
}

void MapWidget::setScaleForDownAnimation(const double &scale)
{
    qDebug() << Q_FUNC_INFO << scale;
    QTransform trans;
    trans.scale(scale, scale);
    QImage temp = m_lastBackgroundImage.transformed(trans);
    m_backgroundImage.fill(Qt::gray);
    int deltaX = (m_backgroundImage.width() - temp.width()) / 2;
    int deltaY = (m_backgroundImage.height() - temp.height()) / 2;
    QPainter p(&m_backgroundImage);
    p.drawImage(deltaX, deltaY, temp);
    update();
}


