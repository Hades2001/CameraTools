#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QTimer>
#include <QDialog>
#include "httpconnect.h"
#include "serialconnect.h"
#include "nofocusframedelegate.h"
#include "imagedialog.h"
#include "burner.h"
#include "QTimer"
#include "clickbtn.h"
#include <QListWidgetItem>
#include <QDateTime>
#include <QFileDialog>
#include "ipdialog.h"
#include "messageboxdialog.h"
#include <QtMath>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum{
        kDeviceStateIDLE = 0,
        kInitDevice,
        kChangeMode,
        ksetDeviceToHttpMode,
        ksetDeviceToSerialMode,
        kDeviceHTTPMode,
        kDeviceSerialMode,
        kDeviceError,
        kDeviceStateMAX,
    };
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString saveImage(QPixmap image, QDateTime time);
    void setConfigFromDataArray(QByteArray);
    void btnConnect();
    void sendCmdToDevicd(quint8 cmd, qint16 data, quint16 retry = 10);
    void changeDevicdMode(int mode);
    void savePhoto();
    void readPhoto();
    void setMainDialogSize(bool mode);

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

private slots:
    void anaCMDData(quint8 cmd,QByteArray dataArray);
    void sendBtnCmdState(quint8 cmd,bool state);
    void on_cb_FrameSize_currentIndexChanged(int index);
    void on_bn_photo_pressed();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_bn_savePath_pressed();
    void on_hs_Brightness_valueChanged(int value);
    void on_hs_Quality_valueChanged(int value);
    void on_hs_Contrast_valueChanged(int value);
    void on_hs_Saturation_valueChanged(int value);
    void on_hs_AELevel_valueChanged(int value);
    void on_hs_Gain_valueChanged(int value);
    void on_bn_openOther_pressed();
    void on_cb_connectMode_currentIndexChanged(int index);
    void on_bn_photo_3_pressed();
    void on_le_timer_textChanged(const QString &arg1);
    void on_bn_timer_pressed();
    void on_Bn_Exit_pressed();
    void on_bn_timer_clicked(bool checked);

    void on_hs_Quality_sliderReleased();

    void on_hs_Brightness_sliderReleased();

    void on_hs_Contrast_sliderReleased();

    void on_hs_Saturation_sliderReleased();

    void on_hs_AELevel_sliderReleased();

    void on_hs_Gain_sliderReleased();

    void on_comboBox_2_currentIndexChanged(int index);

    void on_bn_photo_3_clicked();

private:
    Ui::MainWindow *ui;

    httpconnect *_httpWidget;
    SerialConnect *_serialWidget;

    bool _WindowsOpenFlag = false;
    bool _WindowsMoveFlag = false;
    QPoint m_Dragpostion;
    QPoint m_MousePoint;

    QTimer* _fpsTimer;
    int     _fpsCount = 0;
    int     _fpsValue = 0;

    QPixmap _image;
    QPixmap _imagefullSize;
    QString _savePath;

    bool    _getDeviceModeFlag = false;
    int     _deviceRunState = kInitDevice;

    MessageBoxDialog waitDialog;

    QMap<quint8, clickBtn*> btnMap;

    bool    _imageFlag = false;

    bool    _PhotoTimerFlag = false;
    QTimer *_PhotoTimer;
    int     _timerCount = 0;

    QSettings *_setting;

};

#endif // MAINWINDOW_H
