#ifndef MAINWINDOWMAPSFORARM_H
#define MAINWINDOWMAPSFORARM_H

#include <QMainWindow>

class SlippyMapWidget;

namespace Ui {
class MainWindowMapsForArm;
}

class MainWindowMapsForArm : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowMapsForArm(QString path, QWidget *parent = 0);
    ~MainWindowMapsForArm();

private:
    Ui::MainWindowMapsForArm *ui;
    SlippyMapWidget *m_glMapWidget;

};

#endif // MAINWINDOWMAPSFORARM_H
