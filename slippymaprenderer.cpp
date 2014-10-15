#include "slippymaprenderer.h"
#include "imapdata.h"

#include <QPainter>
#include <QTime>
#include <QDebug>

SlippyMapRenderer::SlippyMapRenderer(QObject *parent) :
    IRenderer(nullptr, parent),
    m_mapDataSource(nullptr)
{
}

void SlippyMapRenderer::setMapDataSource(IMapData * const mapDataSource)
{
    m_mapDataSource = mapDataSource;
}

void SlippyMapRenderer::render(QPainter *painter, QRect rect)
{
    if (!m_mapDataSource) {
        return;
    }
    painter->resetTransform();
    QPoint mapOffset = m_mapDataSource->currentMapOffset();
    int tileX = mapOffset.x() / m_mapDataSource->tileWidth();
    int tileY = mapOffset.y() / m_mapDataSource->tileHeight();
    int addX = 1;
    int addY = 1;
    if (mapOffset.x() % m_mapDataSource->tileWidth() != 0) {
        ++addX;
    }
    if (mapOffset.y() % m_mapDataSource->tileHeight() != 0) {
        ++addY;
    }
    QRect translatedRect = rect;
    translatedRect.translate(mapOffset);
    for (int x = 0; x <= ((rect.x() + rect.width()) / 256 + addX); ++x) {
        for (int y = 0; y <= ((rect.y() + rect.height()) / 256 + addY); ++y) {
            QPoint tilePos(x + tileX, y + tileY);
            QRect tileBox = tileRect(tilePos);
            if (translatedRect.intersects(tileBox)) {
                QRect r = QRect(QPoint(tileBox.x() - mapOffset.x(),
                                tileBox.y() - mapOffset.y()),
                                tileBox.size());
                painter->drawPixmap(r, m_mapDataSource->getTile(tilePos));
            }
        }
    }
}

QRect SlippyMapRenderer::tileRect(QPoint tilePos)
{
    if (!m_mapDataSource) {
        return QRect();
    }
    int tileWidth = m_mapDataSource->tileWidth();
    int tileHeight = m_mapDataSource->tileHeight();
    return QRect(QPoint(tilePos.x() * tileWidth, tilePos.y() * tileHeight),
                 QSize(tileWidth, tileHeight));
}
