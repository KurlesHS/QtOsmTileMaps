#ifndef SLIPPYMAPWIDGET_H
#define SLIPPYMAPWIDGET_H

#include "isetrenderer.h"

#include <QGLWidget>

class IMapData;
class IRenderer;
class SlippyMapRenderer;

class SlippyMapWidget : public QGLWidget, public ISetRenderer
{
    Q_OBJECT
public:
    explicit SlippyMapWidget(QWidget *parent = 0);
    ~SlippyMapWidget() {}
    void setMapDataSource(IMapData * const mapDataSource);
    void zoomUp(const QPoint &centerTo);
    void zoomDown(const QPoint &centerTo);
    void centerMap();
    void setRenderer(IRenderer *renderer);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void handleMouseMoveWithLeftButtonPressed();

signals:

public slots:

private:
    IMapData *m_mapDataSource;
    bool m_isLeftMousePressed;
    bool m_isUserActionsEnabled;
    QPoint m_mousePressCoord;
    QPoint m_mapDeltaOnMousePress;
    SlippyMapRenderer *m_slippyMapRenderer;
    IRenderer *m_currentRenderer;
};

#endif // SLIPPYMAPWIDGET_H
