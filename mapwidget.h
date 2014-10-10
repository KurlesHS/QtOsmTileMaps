#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QFutureWatcher>


class IMapData;

class MapWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double scaleForDownAnimation READ scaleForDownAnimation WRITE setScaleForDownAnimation)

public:
    explicit MapWidget(QWidget *parent = 0);
    void setMapData(IMapData * const mapDataSource, bool update = true);
    void setMapDelta(QPoint mapDelta);

    int mapDeltaX() const;
    int mapDeltaY() const;


    void centerByGeoCoord(const double &lat, const double &lon);
    void centerByMapOffset(const int offsetX, const int offsetY);
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
    QPointF getCurrentGeoCoordByMousePos(const QPoint &pos) const;

private slots:
    void onCreateBackgroundFinished();
    void enableZooming();
    void valueChanged(QVariant value);
    void onDownAnimationFinished();

private:
    QImage m_backgroundImage;
    QImage m_lastBackgroundImage;
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
