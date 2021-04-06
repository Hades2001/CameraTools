#ifndef SERIALCONNECT_H
#define SERIALCONNECT_H

#include <QWidget>
#include <QListWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QtCharts/QtCharts>
#include <QTimer>
#include <QList>
#include <QImage>
#include <QCoreApplication>
#include <QProcess>
#include <QTimer>
#include "wificonfigdialog.h"

#include "clickedcombobox.h"
#include "nofocusframedelegate.h"
#include "burner.h"
#include "serialrevice.h"
#include "ipdialog.h"
#include "messageboxdialog.h"

namespace Ui {
class SerialConnect;
}

class SerialConnect : public QWidget
{
    Q_OBJECT

public:

    enum eBurnerState{
        kBurnerIDLE,
        kBurnerWait,
        kBurnerWriting,
        kBurnerMAX
    };

public:
    explicit SerialConnect(QWidget *parent = nullptr);
    ~SerialConnect();
    void scanSerialName();
    //void sendCMD(quint8 cmd,QByteArray data){ qDebug()<<"sendCMD"; emit sendData(cmd,data);}
    void sendCMD(quint8 cmd,qint16 data, quint16 retry = 10){ qDebug()<<"sendCMD"; emit sendDataToDevice(cmd,data,retry);}
    void sendCMD(quint8 cmd,QByteArray array, quint16 retry = 10){ qDebug()<<"sendCMD"; emit sendArrayToDevice(cmd,array,retry);}
    void sendSerialCtrl(int state);
    bool isConnect(){ return _isConnected; }
    void setWifiConfig();
    bool getSerialState(){ return _isConnected; }
private:
    Ui::SerialConnect *ui;

signals:
    void sendPixmap(QPixmap);
    void sendImage(QImage);
    void sendSerialConnecting();
    void SerialCtrl(QString PoartName,int state);
    void sendDataToDevice(quint8 cmd, qint16 data, quint16 retry = 10);
    void sendArrayToDevice(quint8 cmd,QByteArray dataArray, quint16 retry = 10);
    void sendArray(quint8 cmd,QByteArray dataArray);
    //void sendData(quint8 cmd, QByteArray dataarray);
private slots:
    void revicePack(quint8 cmd,QByteArray dataArray);

private slots:
    void on_bn_connect_pressed();
    void on_bn_connect_3_pressed();
    void on_bn_setting_pressed();
    void on_bn_getIP_pressed();

    void on_bn_connect_clicked();

private:
    QProcess *   _esptool;

    QTimer*      _burnerTimer;
    int          _burner = kBurnerIDLE;
    int          _burnerCount = 0;

    int          _serialState = SerialRevice::kSerialClose;
    SerialRevice* _serialTh;
    bool         _isConnected = false;
};

#endif // SERIALCONNECT_H
