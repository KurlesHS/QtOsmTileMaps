#include "mapscene.h"
#include "imapdata.h"

#include <QDebug>

MapScene::MapScene(QObject *parent) :
    QObject(parent),
    m_mapOffsetInPx(0, 0)
{

}

void MapScene::render(QPainter &p, const QRect &rect, IMapData * const mapData)
{
    if (!mapData) {
        return;
    }
    int tileX = m_mapOffsetInPx.x() / mapData->tileWidth() + mapData->minX();
    int tileY = m_mapOffsetInPx.y() / mapData->tileHeight() + mapData->minY();
    int addX = 1;
    int addY = 1;
    if (m_mapOffsetInPx.x() % mapData->tileWidth() != 0) {
        --tileX;
        ++addX;
    }
    if (m_mapOffsetInPx.y() % mapData->tileHeight() != 0) {
        --tileY;
        ++addY;
    }
    QRect translatedRect = rect;
    translatedRect.translate(m_mapOffsetInPx);
    for (int x = 0; x <= ((rect.x() + rect.width()) / 256 + addX); ++x) {
        for (int y = 0; y <= ((rect.y() + rect.height()) / 256 + addY); ++y) {
            QPoint tilePos(x + tileX, y + tileY);
            QRect tileBox = tileRect(tilePos, mapData);
            if (translatedRect.intersects(tileBox)) {
                QRect r = QRect(QPoint(tileBox.x() - m_mapOffsetInPx.x(),
                                tileBox.y() - m_mapOffsetInPx.y()),
                                QSize(tileBox.width(),
                                tileBox.height()));
                p.drawPixmap(r, mapData->getTile(tilePos));
            }
        }
    }
}

void MapScene::setMapDelta(const QPoint &mapDelta)
{
    m_mapOffsetInPx = mapDelta;
}

QRect MapScene::tileRect(QPoint tilePos, IMapData *mapData)
{
    QPoint tp = tilePos - QPoint(mapData->minX(), mapData->minY());
    return QRect(QPoint(tp.x() * 0x100, tp.y() * 0x100), QSize(0x100, 0x100));
}
