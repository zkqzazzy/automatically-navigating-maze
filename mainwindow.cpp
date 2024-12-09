#include <QPainter>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "controller.h"
#include <qevent.h>
#include <time.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , canvas(size())
    , remainingTime(188)
    , isHard(0)
    , treasure(0)
    , keys(0)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &MainWindow::tickEvent);

    //进行游戏难度的选择
    QMessageBox MBox;
    MBox.setWindowTitle("迷宫Plus版");
    MBox.setText("请选择迷宫的难度！");
    // MBox.setIconPixmap(oPixmap("c:\\Users|lxiexuewu\\Desktopllicon c.png"));
    QPushButton *simpleBut = MBox.addButton("简单",QMessageBox::AcceptRole);
    QPushButton *hardBut = MBox.addButton("困难",QMessageBox::AcceptRole);
    MBox.exec();
    if(MBox.clickedButton()==(QAbstractButton*)simpleBut){
        controller.newGame("C:/Users/86150/Desktop/maze-zkq/maze/config.json");
    }else{
        isHard = 1;
        controller.newGame("C:/Users/86150/Desktop/maze-zkq/maze/configHard.json");
    }

    // controller.newGame("C:/Users/86150/Desktop/maze-zkq/maze/config.json");

    ddlTime = new QTimer(this);
    ddlTime->start(1000);//1s调用一次

    connect(ddlTime, &QTimer::timeout , this, &MainWindow::onTimeout);

    timer->start(1000 / controller.getFps());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

void MainWindow::paintEvent(QPaintEvent* e)
{
    controller.render(canvas);
    QPainter painter(this);
    painter.drawPixmap(QPoint(0, 0), canvas);
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    qDebug() << e << "\n";//输出键盘事件信息到调试控制台
    controller.handleKeyPress(e);
    QWidget::keyPressEvent(e); // Important to call the base class for other keys
    repaint();
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
    qDebug() << e << "\n";
    controller.handleMousePress(e);
    QMainWindow::mousePressEvent(e);
}

void MainWindow::tickEvent() {
    bool changed = controller.handleTickEvent();
    if(changed) {
        repaint();
    }
}

void MainWindow::onTimeout(){
    // controller.onTimeout(remainingTime,ui,timer);
    --remainingTime;
    if (remainingTime > 0)   // 更新界面显示剩余时间
    {
        // 构建多行文本
        string id = controller.getRoomId();
        string isTreasure = "否";
        string isKeys = "否";
        if(treasure == 1){
            isTreasure = "是";
        }
        if(keys == 1){
            isKeys = "是";
        }
        QString text;
        // if(controller.isTaskRoom()){
        if(isHard){
        if(controller.getTreasure()){
            treasure = 1;
            isTreasure = "是";
        }
        if(controller.getKeys()){
            keys = 1;
            isKeys = "是";
        }
        text = tr("房间id：%1\n剩余时间：%2秒\n是否拣到钥匙：%3\n是否拣到宝箱：%4\n")
                    .arg(QString::fromStdString(id))
                    .arg(remainingTime)
                    .arg(QString::fromStdString(isKeys))
                    .arg(QString::fromStdString(isTreasure));
        } else {
            text = tr("房间id：%1\n剩余时间：%2秒\n")
                        .arg(QString::fromStdString(id))
                        .arg(remainingTime);
        }
        // }else{
        //     text = tr("房间id：%1\n剩余时间：%2秒\n")
        //                .arg(QString::fromStdString(id))
        //                .arg(remainingTime);
        // }
        ui->timeLabel->setText(text);
    }
    else
    {
        ddlTime->stop(); // 停止计时器
        QMessageBox::warning(this, tr("游戏失败"), tr("时间到了！游戏失败！"));
    }
}

void MainWindow::startEasyMode(){
    controller.newGame("C:/Users/86150/Desktop/maze-zkq/maze/config.json");

}

void MainWindow::startHardMode(){
    controller.newGame("C:/Users/86150/Desktop/maze-zkq/maze/configHard.json");
}

