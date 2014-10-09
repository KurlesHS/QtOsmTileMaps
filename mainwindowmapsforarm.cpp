#include "mainwindowmapsforarm.h"
#include "ui_mainwindowmapsforarm.h"

#include "mapwidget.h"
#include "mapdatadisk.h"
#include <QHBoxLayout>
#include <QTimer>

MainWindowMapsForArm::MainWindowMapsForArm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowMapsForArm),
    m_mapWidget(new MapWidget(this))
{
    ui->setupUi(this);
    MapDataDisk *mapSource = new MapDataDisk(
                "D:/Archive/Maps apps/Maperitive/Tiles",
                15, 19804, 19866, 9658, 9683);
    m_mapWidget->setMapData(mapSource);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(m_mapWidget);
    ui->centralWidget->setLayout(lay);
    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this,SLOT(onTimeout()));
    QTimer::singleShot(1000, this, SLOT(forCenterByGeoCoord()));
    t->start(50);

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

void MainWindowMapsForArm::forCenterByGeoCoord()
{
    m_mapWidget->centerByGeoCoord(59.115400, 37.974400);

}


