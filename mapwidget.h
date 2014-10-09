#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QFutureWatcher>

class IMapData;

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = 0);
    void setMapData(IMapData * const mapDataSource);

    int mapDeltaX() const;
    void setMapDeltaX(int mapDeltaX);

    int mapDeltaY() const;
    void setMapDeltaY(int mapDeltaY);

    void setMapDelta(QPoint mapDelta);

    void centerByGeoCoord(const double &lat, const double &lon);
    void centerByMapOffset(const int offsetX, const int offsetY);


protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void setMapDeltaXHelper(int mapDeltaX);
    void setMapDeltaYHelper(int mapDeltaY);
    void createBackground();
    void createBackgroundInThreadAndUpdate(bool update);
    QImage createBackground(const QSize &size);

private slots:
    void onCreateBackgroundFinished();

private:
    QImage m_backgroundImage;
    IMapData *m_mapDataSource;
    QFutureWatcher<QImage> m_futureWatcherForFinishCreateBackground;

    int m_mapDeltaX;
    int m_mapDeltaY;

    QPoint m_pressedMousePoint;
    int m_pressedMouseDeltaX;
    int m_pressedMouseDeltaY;
    bool m_isMousePressed;
    bool m_createBackgroundInProcess;
    bool m_isNeededUpdateAfterCreateBackground;
};

#endif // MAPWIDGET_H
