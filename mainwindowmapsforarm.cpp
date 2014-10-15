#include "mainwindowmapsforarm.h"
#include "ui_mainwindowmapsforarm.h"

#include "mapdatadisk.h"
#include "slippymapwidget.h"
#include "mapdatambtiles.h"

#include <QHBoxLayout>
#include <QTimer>
#include <QMessageBox>

MainWindowMapsForArm::MainWindowMapsForArm(QString path, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowMapsForArm),
    m_glMapWidget(new SlippyMapWidget(this))
{
    ui->setupUi(this);
    IMapData *mapSource;
    if (path.endsWith(".mbtiles")) {
        mapSource = new MapDataMbtiles(path);
    } else {
        mapSource = new MapDataDisk(path);
    }
    if (!mapSource->isValidData()) {
        QMessageBox::critical(this, tr("error!"), tr("can't load data source \"%1\"").arg(path));
        QTimer::singleShot(1, qApp, SLOT(quit()));
    }
    mapSource->setZoomLvlToMin();
    mapSource->toTopLeftCorner();
    m_glMapWidget->setMapDataSource(mapSource);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(m_glMapWidget);
    ui->centralWidget->setLayout(lay);
}

MainWindowMapsForArm::~MainWindowMapsForArm()
{
    delete ui;
}



