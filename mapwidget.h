#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QFutureWatcher>
#include <QVariantAnimation>

class IMapData;

struct ZoomAnimationHelper {
    double scaleFactor;
    QSize position;
};

Q_DECLARE_METATYPE(ZoomAnimationHelper)

class ZoomAnimation : public QVariantAnimation {
public:
    ZoomAnimation(QObject *parent = 0) : QVariantAnimation(parent) {}
    virtual ~ZoomAnimation() {}

    // QVariantAnimation interface
protected:
    void updateCurrentValue(const QVariant &value) {emit valueChanged(value);}
    QVariant interpolated(const QVariant &from, const QVariant &to, qreal progress) const {
        ZoomAnimationHelper zahFrom = from.value<ZoomAnimationHelper>();
        ZoomAnimationHelper zahTo = to.value<ZoomAnimationHelper>();
        ZoomAnimationHelper res;
        res.scaleFactor = (zahTo.scaleFactor - zahFrom.scaleFactor) * progress + zahFrom.scaleFactor;
        res.position = (zahTo.position - zahFrom.position) * progress + zahFrom.position;
        return QVariant::fromValue<ZoomAnimationHelper>(res);
    }

};

class MapWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double scaleForDownAnimation READ scaleForDownAnimation WRITE setScaleForDownAnimation)

    enum States {
        NormalState,
        FadeDownState,
        SkipUpdateState
    };

public:
    explicit MapWidget(QWidget *parent = 0);
    void setMapData(IMapData * const mapDataSource, bool update = true);
    void setMapDelta(QPoint mapDelta, const bool update = true);

    int mapDeltaX() const;
    int mapDeltaY() const;

    void centerByGeoCoord(const double &lat, const double &lon);
    void centerByMapOffset(const int offsetX, const int offsetY);

    QPoint getMapDeltaByCenterGeoCoord(const double &lat, const double &lon);
    QPoint getPosInByGeoCoords(const double &lat, const double &lon);

    double scaleForDownAnimation() const;
    void setScaleForDownAnimation(const double &scale);


protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void setMapDeltaXHelper(int mapDeltaX);
    void setMapDeltaYHelper(int mapDeltaY);
    void createBackground();
    void createBackgroundInThreadAndUpdate(bool update);
    QImage createBackground(const QSize &size);
    QPointF getCurrentGeoCoordByPos(const QPoint &pos) const;
    void continueFadeDownAnimation(const int currentTick, const int maxTick, const int delta);

private slots:
    void onCreateBackgroundFinished();
    void enableZooming();
    void onDownAnimationFinished();
    void onFadeAnimation();
    void onFadeResult();
    void currentLoopParalelGroupAnimationChanged(const int loop);
    void onZoomDownAnimationTick(const QVariant &value);

private:
    States m_currentState;
    QImage m_backgroundImage;
    QImage m_lastBackgroundImage;

    QImage m_fadeImage1;
    QImage m_fadeImage2;

    IMapData *m_mapDataSource;
    QFutureWatcher<QImage> m_futureWatcherForFinishCreateBackground;
    QPointF m_centerTo;

    QPoint m_mapDelta;
    QPoint m_screenOffset;

    QPoint m_pressedMousePoint;
    QPoint m_pressedMouseDelta;

    bool m_isMousePressed;
    bool m_createBackgroundInProcess;
    bool m_isNeededUpdateAfterCreateBackground;
    bool m_isNeededToRecreatebackground;
    bool m_isZoomingEnabled;
};

#endif // MAPWIDGET_H
