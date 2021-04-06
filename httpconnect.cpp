#include "httpconnect.h"
#include "ui_httpconnect.h"

httpconnect::httpconnect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::httpconnect)
{
    ui->setupUi(this);
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    ui->le_ipInput->setValidator(new QRegExpValidator(regExp,ui->le_ipInput));

    manager = new QNetworkAccessManager(this);
    _cmdManager = new QNetworkAccessManager(this);

    connect(_cmdManager, &QNetworkAccessManager::finished, this, &httpconnect::requestFinished );
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply){
        //reply->close();
        Q_UNUSED(reply)
        qDebug()<<"finished";
    });

    _sendState = false;

    reConnectTimer = new QTimer();

    connect(reConnectTimer,&QTimer::timeout,this,[=](){
        switch( _httpState )
        {
            case kHttpIDLE:         break;
            case kHttpConnecting:
                _connectTimeCount++;
                if( _connectTimeCount > 500 )
                {
                    _connectTimeCount = 0;
                    _httpState = kHttpConnectReTry;
                    setConnectClose();
                    delete reply;
                }
                break;
            case kHttpConnected:
                _connectRetryCount = 0;
                _connectTimeCount++;
                if( _connectTimeCount > 500 )
                {
                    //waitDialog.WaitMsgBos("reconnect",
                    //                      "connect to Device");
                    _connectTimeCount = 0;
                    _httpState = kHttpConnectReTry;
                    setConnectClose();
                }
                break;
            case kHttpConnectReTry:
                _connectRetryCount++;
                if( _connectRetryCount > 10 )
                {
                    qDebug()<<"Retry Error";
                    ui->bn_connect->setText("connect");
                    setConnectClose();
                    delete reply;
                    _httpState = kHttpCloes;
                }
                else
                {
                    qInfo("Retry Number:%d",_connectRetryCount);
                    connectTODevice();
                    _httpState = kHttpConnecting;
                }
                break;
            case kHttpConnectError:
                _connectTimeCount = 0;
                _httpState = kHttpConnectReTry;
                break;
            case kHttpSetToClose:

                break;
            case kHttpCloes:

                break;
        }
    });

    reConnectTimer->start(10);
}

httpconnect::~httpconnect()
{
    delete ui;
}

void httpconnect::doProcessReadyRead()
{
    QByteArray dataArray = reply->readAll();
    _httpDataArray.append(dataArray);
    int index = 0;
    int pos = 0;
    do{
        if(( _httpFSM == kIDLE ) && ( index = _httpDataArray.indexOf("Content-Type: image/jpeg\r\nContent-Length:")) != -1)
        {

            //int index = dataArray.indexOf("Content-Type: image/jpeg\r\nContent-Length:");
            //qDebug()<<"------------------------------";

            //QRegExp rx(QString("Content-Type: image/jpeg\r\nContent-Length:\\s(\\d+)\r\n\r\n"));
            //QString str(dataArray);
            //int pos = str.indexOf(rx);
            //if( pos != -1 )
            //{
            //     qDebug()<<"------------------------------";
            //     //qDebug()<<rx.cap(0);
            //     qDebug()<<"length:"<<rx.cap(1);
            //}
            _startIndex = _httpDataArray.indexOf("\r\n\r\n",index) + 4;
            if( _startIndex == -1 )
            {
                qInfo("_startIndex error");
                continue;
            }
            else
            {
                _httpDataArray = _httpDataArray.mid(_startIndex);
                //qInfo("index: %d",index);
                //qInfo("_startIndex: %d",_startIndex);
            }
            //qDebug()<<dataArray.mid(datapos,100);
            //_imageDataArray.append(dataArray.mid(datapos));
            _httpFSM = kData;
        }

        if(( _httpFSM == kData )&&( index = _httpDataArray.indexOf("\r\n--123456789000000000000987654321\r\n")) != -1)
        {
            _endIndex = index;
            //qInfo("_endIndex: %d",_endIndex);
            _imageDataArray.append(_httpDataArray.left(_endIndex ));
            //_imageDataArray.append(_httpDataArray.mid(_startIndex,_endIndex - _startIndex));
            //qInfo("size %d",_imageDataArray.size());
            //qDebug()<<"------------------------------end";

            QImage image;
            image.loadFromData(_imageDataArray);
            emit sendImage(image);
            _imageDataArray.clear();
            //qDebug()<<_imageDataArray.left(50);
            //qDebug()<<_imageDataArray.right(50);

            _httpDataArray = _httpDataArray.mid(_endIndex +
                                                QString("\r\n--123456789000000000000987654321\r\n").size());
            _startIndex = 0;
            _endIndex = 0;
            pos = 0;
            _httpFSM = kIDLE;
        }
    }while( index != -1 );
    _httpState = kHttpConnected;
    _connectTimeCount = 0;
}

void httpconnect::on_bn_connect_pressed()
{
    qDebug()<<_httpState;
    if( _httpState != kHttpIDLE && _httpState != kHttpCloes )
    {
        if( reply != nullptr)
        reply->close();
    }
    else
    {
        connectTODevice();
    }
}
void httpconnect::connectTODevice()
{
    if( _httpState == kHttpConnected )
    {
        qDebug()<<"Close";
        if( reply != nullptr)
        reply->close();
    }
    else
    {
        QNetworkRequest requst;
        //requst.setRawHeader(QByteArray("User-Agent"), QByteArray("Sun, 06 Nov 1994 08:49:37 GMT"));   //设置头
        QString url_str = "http://"+ui->le_ipInput->text() + ":81";
        qDebug()<<url_str;
        QUrl url;
        url.setUrl(url_str);
        requst.setUrl(url);
        reply = manager->get(requst);

        _httpState = kHttpConnecting;

        connect(reply, &QIODevice::readyRead, this, &httpconnect::doProcessReadyRead);
        connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),    //收到异常信号
               [=](QNetworkReply::NetworkError code)
                 {
                    qDebug() << "NetworkError"<<code;
                     switch(int(code))
                     {
                         case QNetworkReply::ConnectionRefusedError:
                             qDebug() << "远程服务器拒绝连接（服务器不接受请求）";
                             break;
                         case QNetworkReply::HostNotFoundError:
                             qDebug() << "找不到远程主机名（无效的主机名）";
                             break;
                         case QNetworkReply::TimeoutError:
                             qDebug() << "与远程服务器的连接超时";
                             break;
                         default:
                             break;
                     }

                     //_httpState = kHttpConnectError;
                 });
        connect(reply, &QNetworkReply::finished,this,[=](){
            qDebug() <<"End";
            ui->bn_connect->setText("connect");
            //if( _httpState == kHttpConnected )
            {
                _httpState = kHttpCloes;
                delete reply;
            }

         });

        ui->bn_connect->setText("close");
        sendCMD(29,1);
    }
}

void httpconnect::setConnectClose()
{
    if( _httpState == kHttpConnected )
    {
        if( reply != nullptr)
        reply->close();
    }
}

void httpconnect::sendCMDToDevice_URL( QString url )
{
    QNetworkRequest request;
    qDebug()<<url;
    request.setUrl(url);
    _cmdManager->get(request);
}

void httpconnect::requestFinished(QNetworkReply* reply)
{
    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
    }
    else {
        // 获取返回内容
        QByteArray dataArray = reply->readAll();
        qDebug() << dataArray;
        quint8 cmd = dataArray.at(0) & 0x7f;
        emit sendArray(cmd,dataArray.mid(1));
        if( !sendList.isEmpty())
        {
            sendList.removeFirst();
            if( !sendList.isEmpty())
            {
                sendCMDToDevice_URL(sendList.first());
            }
            else
            {
                _sendState = false;
            }
        }
        else
        {
            _sendState = false;
        }
    }
}

void httpconnect::sendCMDToDevice(quint8 cmd,qint16 data)
{
    QNetworkRequest request;
    QString url = "http://"+ui->le_ipInput->text() + ":80";
    url += QString("/config?cmd=%1&value=%2").arg(cmd).arg(data);
    sendList.append(url);
    qDebug()<<_sendState;
    if( _sendState == false )
    {
        sendCMDToDevice_URL(sendList.first());
        _sendState = true;
    }
}

void httpconnect::sendCMD(quint8 cmd,qint16 data)
{
    if( _httpState == kHttpConnected )
    {
        qDebug()<<"sendHttpData";
        sendCMDToDevice(cmd,data);
    }
}

void httpconnect::setSSID( QString ssid )
{
    Q_UNUSED(ssid);
    ui->le_ssid->setText(ssid);

}
void httpconnect::setIPAndConnect(QString ip,bool connect)
{
    ui->le_ipInput->setText(ip);
    if( connect )
    {
        connectTODevice();
    }
}

