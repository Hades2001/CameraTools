#include "serialrevice.h"

SerialRevice::SerialRevice(QObject *parent) : QObject(parent)
{
    _serial = new QSerialPort;

    connect(_serial,&QSerialPort::readyRead,this,&SerialRevice::serialReadready,Qt::AutoConnection);
    connect(_serial,&QSerialPort::errorOccurred,this,[=](QSerialPort::SerialPortError error){
        if( error == QSerialPort::ResourceError)
        {
            _sendTimer->stop();
            _serial->close();
            _dataPack.DataFSM = kIDLE;

            emit serialError();
        }
    },Qt::AutoConnection);

    _sendTimer = new QTimer();

    connect(_sendTimer,&QTimer::timeout,this,[=](){
        if( !sendList.isEmpty())
        {
            dataPack_t send = sendList.first();
            if( send.sendCount == 0 )
            {
                sendList.removeFirst();
                return;
            }
            if( _serial->isOpen())
            {
                QString datastr;
                for( int i = 0; i < send.RAWData.size(); i++ )
                {
                    datastr += QString("%1 ").arg(quint8(send.RAWData.at(i)%0xff),2,16,QLatin1Char('0'));
                }
                qDebug()<<datastr;
                _serial->write(send.RAWData);
                send.sendCount --;
                sendList[0] = send;
            }
        }
    },Qt::AutoConnection);

    _reviceThread = new QThread();

    this->moveToThread(_reviceThread);
    _serial->moveToThread(_reviceThread);
    _sendTimer->moveToThread(_reviceThread);
    _reviceThread->start();
}

void SerialRevice::SerialCtrl(QString PoartName, int state)
{
    if( state == kSerialOpen )
    {
        if( !_serial->isOpen())
        {
            _serial->setBaudRate(1500000,QSerialPort::AllDirections);
            _serial->setDataBits(QSerialPort::Data8);
            _serial->setFlowControl(QSerialPort::NoFlowControl);
            _serial->setParity(QSerialPort::NoParity);
            _serial->setStopBits(QSerialPort::OneStop);
            _serial->setPortName(PoartName);
            _serial->setDataTerminalReady(true);
            _serial->setRequestToSend(true);
            if( !_serial->open(QIODevice::ReadWrite))
            {
                return ;
            }
            _dataPack.DataFSM = kIDLE;
            _sendTimer->start(500);
            emit serialState(kSerialOpen);
        }
    }
    else
    {
        if( _serial->isOpen())
        {
            _serial->close();
            _sendTimer->stop();
            emit serialState(kSerialClose);
        }
    }
}

quint16 SerialRevice::addListToList(quint8 cmd,QByteArray data, quint16 retry)
{
    dataPack_t send;
    send.RAWData.clear();
    send.RAWData.append(char(0xAA));
    send.RAWData.append(char(0x55));

    send.Length = quint32(data.size()) + 2;
    send.CMD = cmd;

    send.RAWData.append(char(send.Length >> 24));
    send.RAWData.append(char(send.Length >> 16));
    send.RAWData.append(char(send.Length >> 8));
    send.RAWData.append(char(send.Length >> 0));

    send.LengthXOR = 0x00;
    for( int i = 2; i < 6; i++ )
    {
        send.LengthXOR ^= send.RAWData.at(i);
    }

    send.RAWData.append(char(send.LengthXOR));

    send.RAWData.append(char(cmd));
    send.RAWData.append(data);

    send.XORData = 0x00;

    for( int i = 0; i < send.RAWData.size(); i++ )
    {
        send.XORData ^= send.RAWData.at(i);
    }

    send.RAWData.append(char(send.XORData));

    send.sendCount = retry;

    _sendID ++;
    send.id = _sendID;

    sendList.append(send);

    return _sendID;

}

void SerialRevice::sendData(quint8 cmd, quint16 data, quint16 retry)
{
    if( !_serial->isOpen()) return;
    qDebug()<<"sendData";
    QByteArray dataArray;
    dataArray.append(char(data & 0xff ));
    dataArray.append(char(data >> 8 ));
    addListToList(cmd,dataArray,retry);
}
void SerialRevice::sendArray(quint8 cmd, QByteArray dataarray, quint16 retry)
{
    qDebug()<<"sendArray";
    addListToList(cmd,dataarray,retry);
}

void SerialRevice::serialReadready()
{
    QByteArray dataArray = _serial->readAll();
    quint8 data = 0;
    if( dataArray.size() <= 0 ) return;

    for( int i = 0; i < dataArray.size(); i++ )
    {
        data = quint8(dataArray.at(i)) & 0xff;
        switch( _dataPack.DataFSM )
        {
            case kIDLE:  _dataPack.DataFSM = ( data == 0xAA ) ? kPack : kIDLE; break;
            case kPack: _dataPack.DataFSM = ( data == 0x55 ) ? kpackZ : kIDLE; break;
            case kpackZ:
                if( data == 0x00 )
                {
                    _dataPack.XORData = 0x00 ^ 0xAA ^ 0x55;
                    _dataPack.LengthXOR = 0x00;
                    _dataPack.DataFSM = kLength;
                    _dataPack.count = 0;
                    _dataPack.Length = 0;
                }
                else
                {
                    _dataPack.DataFSM = kIDLE;
                }
                break;
            case kLength:
                _dataPack.Length = ( _dataPack.Length * 256 ) + data;
                _dataPack.XORData ^= data;
                _dataPack.LengthXOR ^= data;
                _dataPack.count ++;
                if( _dataPack.count >= 3 )
                {
                    _dataPack.DataFSM = kLengthXOR;
                    _dataPack.count = 0;
                }
                break;
            case kLengthXOR:
                _dataPack.XORData ^= data;
                if( data == _dataPack.LengthXOR )
                {
                    _dataPack.DataFSM = kCMD;
                    qInfo("Length : %d",_dataPack.Length);
                }
                else
                {
                    _dataPack.DataFSM = kIDLE;
                    qDebug()<<"Length XOR Error";
                }
                break;
            case kCMD:
                _dataPack.CMD = data;
                _dataPack.XORData ^= data;
                qInfo("CMD : %02X",_dataPack.CMD);
                _dataPack.Length -= 2;
                _dataPack.DataFSM = kData;
                _dataPack.DataArray.clear();
                _dataPack.count = 0;

                break;
            case kData:
                _dataPack.DataArray.append(char(data));

                _dataPack.XORData ^= data;
                _dataPack.count ++;
                if( _dataPack.count >= _dataPack.Length )
                {
                    _dataPack.DataFSM = kEnd;
                }
                break;
            case kEnd:
                if( _dataPack.XORData == data )
                {
                    if( _dataPack.CMD != 0x00 )
                    {
                        QString hexstr;
                        for(int i = 0 ; i< _dataPack.DataArray.size();i++)
                        {
                            hexstr += QString(" %1").arg(quint8(_dataPack.DataArray.at(i)&0xff),2,16,QLatin1Char('0'));
                        }
                        qDebug()<<"data:"<<hexstr;

                        if( !sendList.isEmpty())
                        {
                            dataPack_t pack = sendList.first();
                            _dataPack.CMD &= 0x7f ;
                            if( pack.CMD == _dataPack.CMD )
                            {
                                sendList.removeFirst();
                            }
                        }
                    }
                    emit revicePack(_dataPack.CMD,_dataPack.DataArray);
                }
                else
                {
                    qDebug()<<"XOR Error"<<QString(" XOR:%1").arg(data,2,16,QLatin1Char('0'));
                }
                _dataPack.DataFSM = kIDLE;
                break;
            default: _dataPack.DataFSM = kIDLE; break;
        }
    }
}


