#include "wificonfigdialog.h"
#include "ui_wificonfigdialog.h"

wifiConfigDialog::wifiConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wifiConfigDialog)
{
    ui->setupUi(this);
    QString _SSDIStr;
    QString _passwdStr;
}

wifiConfigDialog::~wifiConfigDialog()
{
    delete ui;
}

QByteArray wifiConfigDialog::getWifiConfigArray(QString ssid, QString pwd)
{
    ui->le_ssid->setText(ssid);
    ui->le_pwd->setText(pwd);
    _SSIDStr = ssid;
    _passwdStr = pwd;

    this->setWindowTitle("Wifi config");
    this->exec();
    QByteArray data;
    data.append(char(_SSIDStr.size()) + 1);
    data.append(_SSIDStr.toLocal8Bit());
    data.append('\0');
    data.append(char(_passwdStr.size()) + 1);
    data.append(_passwdStr.toLocal8Bit());
    data.append('\0');

    QString hexstr;
    for(int i = 0 ; i< data.size();i++)
    {
        hexstr += QString(" %1").arg(quint8(data.at(i)&0xff),2,16,QLatin1Char('0'));
    }
    qDebug()<<hexstr;
    return data;
}

void wifiConfigDialog::on_le_pwd_textChanged(const QString &arg1)
{
     _passwdStr = arg1;
}

void wifiConfigDialog::on_le_ssid_textChanged(const QString &arg1)
{
    _SSIDStr = arg1;
}

void wifiConfigDialog::on_bn_ok_pressed()
{
    this->close();
}
