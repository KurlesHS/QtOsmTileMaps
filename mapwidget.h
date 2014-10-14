#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QFutureWatcher>
#include <QVariantAnimation>
#include <QTimer>

class IMapData;
class MapRenderer;
class QThread;

class MapWidget : public QWidget
{
    Q_OBJECT

    enum States {
        NormalState,
        FadeDownState,
        SkipUpdateState
    };

public:
    explicit MapWidget(QWidget *parent = 0);
    virtual ~MapWidget();
    void setMapData(IMapData * const mapDataSource);
    void zoomUp(const QPoint &centerTo);
    void zoomDown(const QPoint &centerTo);
    void centerMap();

public Q_SLOTS:
    void updatedImage(const QImage &image);
    void onResizeTimeout();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void handleMouseMoveWithLeftButtonPressed();

private:
    QImage m_background;
    MapRenderer *m_mapRenderer;
    QThread *m_threadForRenderer;
    QTimer *m_timerForResizeEvent;
    bool m_isLeftMousePressed;
    QPoint m_mousePressCoord;
    QPoint m_mapDeltaOnMousePress;

};

#endif // MAPWIDGET_H
