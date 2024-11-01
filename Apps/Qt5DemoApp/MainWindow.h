#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <Qt3DExtras>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DInput>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Qt3DExtras::QPhongMaterial* material;
    Qt3DExtras::Qt3DWindow* view;
    Qt3DRender::QPointLight* light;


protected:
    void keyReleaseEvent(QKeyEvent* event) override;
private:
    QTimer timer;
    QTimer modeltimer;
    Qt3DCore::QTransform* modeltransform;
    bool isStop = true;
    QSerialPort serial;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
