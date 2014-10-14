#include "mainwindowmapsforarm.h"
#include "ui_mainwindowmapsforarm.h"

#include "mapwidget.h"
#include "mapdatadisk.h"
#include "slippymapwidget.h"

#include <QHBoxLayout>
#include <QTimer>

MainWindowMapsForArm::MainWindowMapsForArm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowMapsForArm),
    //m_mapWidget(new MapWidget(this)),
    m_glMapWidget(new SlippyMapWidget(this))
{
    ui->setupUi(this);

    //MapDataDisk *mapSource = new MapDataDisk("D:/Archive/Maps apps/Maperitive/Tiles");
    MapDataDisk *mapSource = new MapDataDisk("D:/Archive/Maps apps/Maps/cherepovets-tiles");
    //MapDataDisk *mapSource = new MapDataDisk("D:/Archive/gis/Software/Maperitive/Tiles");
    //MapDataDisk *mapSource = new MapDataDisk("D:/Archive/gis/Software/Maperitive/Tiles/tiles.json");
    mapSource->setZoomLvlToMin();
    mapSource->toTopLeftCorner();
    //m_mapWidget->setMapData(mapSource);
    m_glMapWidget->setMapDataSource(mapSource);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(m_glMapWidget);
    ui->centralWidget->setLayout(lay);
    //QTimer *t = new QTimer(this);
    //connect(t, SIGNAL(timeout()), this,SLOT(onTimeout()));
    //t->start(50);

}

MainWindowMapsForArm::~MainWindowMapsForArm()
{
    delete ui;
}

void MainWindowMapsForArm::onTimeout()
{
    //m_mapWidget->setMapDeltaX(m_mapWidget->mapDeltaX() + 1);
    //m_mapWidget->setMapDeltaY(m_mapWidget->mapDeltaY() + 1);
}




