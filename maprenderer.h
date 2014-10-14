#ifndef MAPRENDERER_H
#define MAPRENDERER_H

#include <QObject>
#include <QSize>
#include <QImage>
#include <QMetaType>
#include <QVariantAnimation>
#include <QTime>

class IMapData;
class MapScene;

struct ZoomAnimationHelper {
    double scaleFactor;
    QSize position;
    QRect rect;
};

Q_DECLARE_METATYPE(ZoomAnimationHelper)

class ZoomAnimation : public QVariantAnimation {
    Q_OBJECT
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
        res.rect.setTopLeft((zahTo.rect.topLeft() - zahFrom.rect.topLeft()) * progress + zahFrom.rect.topLeft());
        res.rect.setSize((zahTo.rect.size() - zahFrom.rect.size()) * progress + zahFrom.rect.size());
        return QVariant::fromValue<ZoomAnimationHelper>(res);
    }
};

class MapRenderer : public QObject
{
    Q_OBJECT

    enum States {
        NormalState,
        ZoomUpAnimationState,
        ZoomDownAnimationState,
        FadeAnimationState
    };

public:
    explicit MapRenderer(QObject *parent = 0);
    Q_INVOKABLE void update();
    void setMapDelta(const QPoint &pos, bool update = true);
    QPoint mapDelta() const;
    Q_INVOKABLE void zoomUp(const QPoint &centerTo);
    Q_INVOKABLE void zoomDown(const QPoint &centerTo);
    Q_INVOKABLE void setMapData(QObject *mapData);
    Q_INVOKABLE void setSize(const QSize &size);
    Q_INVOKABLE void centerMap();

private:
    void updateBackground();
    void handleZoomUpState(const ZoomAnimationHelper &zah);
    void handleZoomDownState(const ZoomAnimationHelper &zah);
    void handleFadeAnimationState(const ZoomAnimationHelper &zah, const ZoomAnimationHelper &zahEndValue);
    void startFadeAnimation();
    void connectZoomAnimation(ZoomAnimation *za);


signals:
    void updateImage(const QImage &image);

public slots:
    void enableZoom();
    void onAnimation(const QVariant &value);
    void onAnimationFinished();

private:
    IMapData *m_mapData;
    QSize m_size;
    QImage m_background;
    QImage m_imageForZoom;
    MapScene *m_mapScene;
    bool m_isBusy;
    QPoint m_delayedMapDelta;
    bool m_isZoomEnabled;
    States m_currentState;

};

#endif // MAPRENDERER_H
