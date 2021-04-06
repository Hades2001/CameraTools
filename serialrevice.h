#ifndef SERIALREVICE_H
#define SERIALREVICE_H

#include <QObject>
#include <QImage>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QDebug>

class SerialRevice : public QObject
{
    Q_OBJECT
public:
    explicit SerialRevice(QObject *parent = nullptr);
    void setSerialPortName(QString poartname){ _PoartName = poartname;}
    quint16 addListToList(quint8 cmd, QByteArray data, quint16 retry = 10 );

    enum dataState{
        kIDLE,
        kPack,
        kpackZ,
        kLength,
        kLengthXOR,
        kCMD,
        kData,
        kEnd,
        kMAX
    };

    enum serialState{
        kSerialOpen,
        kSerialClose,
    };

    typedef struct
    {
        QByteArray RAWData;
        dataState  DataFSM = kIDLE;
        quint32    Length = 0;
        quint8     LengthXOR;
        quint8     CMD;
        quint8     XORData = 0x00;
        QByteArray DataArray;
        quint32    count;
        QImage     image;

        quint16    id;
        quint16    sendCount;

    }dataPack_t;

signals:
    void revicePack(quint8 cmd,QByteArray dataArray);
    void serialError();
    void serialState(int);
public slots:
    void serialReadready();
    void SerialCtrl(QString PoartName,int state);
    void sendData(quint8 cmd, quint16 data, quint16 retry = 10);
    void sendArray(quint8 cmd, QByteArray dataarray, quint16 retry = 10);
    //void sendData(quint8 cmd, QByteArray dataarray);
public:
    QString      _PoartName;
    QSerialPort* _serial;
    dataPack_t   _dataPack;
    QThread*     _reviceThread;

    QList<dataPack_t> sendList;
    QTimer*      _sendTimer;
    quint16      _sendID = 0;
};

#endif // SERIALREVICE_H
