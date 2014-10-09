#ifndef MAINWINDOWMAPSFORARM_H
#define MAINWINDOWMAPSFORARM_H

#include <QMainWindow>

class MapWidget;

namespace Ui {
class MainWindowMapsForArm;
}

class MainWindowMapsForArm : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowMapsForArm(QWidget *parent = 0);
    ~MainWindowMapsForArm();

private slots:
    void onTimeout();
    void forCenterByGeoCoord();

private:
    Ui::MainWindowMapsForArm *ui;
    MapWidget *m_mapWidget;
};

#endif // MAINWINDOWMAPSFORARM_H
