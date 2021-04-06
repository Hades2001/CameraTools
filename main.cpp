#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    //QString outstr = ("Writing at 0x0007d000... (96 %)");
    QRegExp rx(QString("Writing at 0x(.*)\\.\\.\\.\\s\\((\\d+)\\s\\%\\)"));


    MainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint);//WindowCloseButtonHint
    w.show();

    return a.exec();
}
