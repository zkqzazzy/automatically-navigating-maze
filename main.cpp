#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);//初始化
    MainWindow w;
    w.show();
    return a.exec();
}
