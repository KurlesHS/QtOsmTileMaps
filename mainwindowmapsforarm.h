#ifndef MAINWINDOWMAPSFORARM_H
#define MAINWINDOWMAPSFORARM_H

#include <QMainWindow>

class MapWidget;
class SlippyMapWidget;

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

private:
    Ui::MainWindowMapsForArm *ui;
    MapWidget *m_mapWidget;
    SlippyMapWidget *m_glMapWidget;

};

#endif // MAINWINDOWMAPSFORARM_H
