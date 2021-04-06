#ifndef WIFICONFIGDIALOG_H
#define WIFICONFIGDIALOG_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class wifiConfigDialog;
}

class wifiConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit wifiConfigDialog(QWidget *parent = nullptr);
    ~wifiConfigDialog();
    QByteArray getWifiConfigArray(QString ssid = "cam",QString pwd = "12345678");

    QString _SSIDStr = "cam";
    QString _passwdStr = "12345678";

private slots:
    void on_le_pwd_textChanged(const QString &arg1);

    void on_le_ssid_textChanged(const QString &arg1);

    void on_bn_ok_pressed();

private:
    Ui::wifiConfigDialog *ui;
};

#endif // WIFICONFIGDIALOG_H
