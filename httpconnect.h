#ifndef HTTPCONNECT_H
#define HTTPCONNECT_H

#include <QWidget>
#include <QRegExpValidator>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QTimer>
#include "messageboxdialog.h"

namespace Ui {
class httpconnect;
}

class httpconnect : public QWidget
{
    Q_OBJECT

public:
    explicit httpconnect(QWidget *parent = nullptr);
    ~httpconnect();

    enum httpFSM{
        kIDLE,
        kHead,
        kData,
        kEnd,
    };

    enum{
        kHttpIDLE = 0,
        kHttpConnecting,
        kHttpConnected,
        kHttpConnectReTry,
        kHttpConnectError,
        kHttpSetToClose,
        kHttpCloes
    };

    void sendCMD(quint8 cmd,qint16 data);
    void sendCMDToDevice(quint8 cmd,qint16 data);
    void sendCMDToDevice_URL(QString);
    void setSSID( QString ssid );
    void setIPAndConnect(QString ip,bool connect = true);
    bool getConnectState(){ return _connect; }
    void setConnectClose();

private:
    Ui::httpconnect *ui;

signals:
    void sendPixmap(QPixmap);
    void sendImage(QImage);
    void sendArray(quint8 cmd,QByteArray dataArray);

private slots:
    void requestFinished(QNetworkReply*);
    void on_bn_connect_pressed();
    void doProcessReadyRead();
    void connectTODevice();

private:
    QNetworkAccessManager*  manager;
    QNetworkReply*          reply;

    QByteArray     _imageDataArray;
    QByteArray     _httpDataArray;
    int            _httpFSM = kIDLE;
    int            _startIndex = 0,_endIndex = 0;
    bool           _connect = false;
    bool           _sendState = false;

    QTimer         *reConnectTimer = nullptr;
    quint16        _connectTimeCount = 0;
    quint16        _connectRetryCount = 0;
    int            _httpState = kHttpIDLE;

    QNetworkAccessManager*  _cmdManager;
    QNetworkReply*          _cmdReply;

    MessageBoxDialog waitDialog;

    QTimer* _sendTimer;

    QList<QString> sendList;
};

#endif // HTTPCONNECT_H
