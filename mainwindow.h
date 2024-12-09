#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "controller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void tickEvent();//定时器事件
    void onTimeout();//到时间退出
    void startEasyMode();
    void startHardMode();

private:
    Ui::MainWindow *ui;
    QTimer* timer;
    QTimer* ddlTime;//时间倒计时
    int remainingTime;//倒计时5分钟
    QPixmap canvas;
    Controller controller;
    int isHard;
    int treasure;
    int keys;
};
#endif // MAINWINDOW_H
