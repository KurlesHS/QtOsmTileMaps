#include "mainwindowmapsforarm.h"
#include "ui_mainwindowmapsforarm.h"

#include "mapdatadisk.h"
#include "slippymapwidget.h"

#include <QHBoxLayout>
#include <QTimer>

MainWindowMapsForArm::MainWindowMapsForArm(QString path, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowMapsForArm),
    m_glMapWidget(new SlippyMapWidget(this))
{
    ui->setupUi(this);
    if (path.isEmpty()) {
        path = "D:/Archive/Maps apps/Maps/cherepovets-tiles";
    }
    MapDataDisk *mapSource = new MapDataDisk(path);
    mapSource->setZoomLvlToMin();
    mapSource->toTopLeftCorner();
    m_glMapWidget->setMapDataSource(mapSource);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(m_glMapWidget);
    ui->centralWidget->setLayout(lay);
#if 0
    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this,SLOT(update());
    t->start(50);
#endif

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




