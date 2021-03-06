#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _httpWidget = new httpconnect(this);
    _serialWidget = new SerialConnect(this);

    ui->lab_connectMode->hide();
    ui->lab_connectMode_2->hide();

    _fpsTimer = new QTimer();

    _fpsTimer->start(1000);
    connect(_fpsTimer, &QTimer::timeout, this, [=](){
        _fpsValue = _fpsCount ;
        _fpsCount = 0;
    });

    //this->setStyleSheet("QMainWindow#MainWindow{background-color: rgb(34, 40, 49);}");

    ui->le_timer->setValidator(new QIntValidator(0, 86400, this));

    connect( _httpWidget,&httpconnect::sendImage,this,[=](QImage image){
        _imagefullSize = QPixmap::fromImage(image);
        if(( _imagefullSize.width() == 0 )||( _imagefullSize.height() == 0 )) return;
        QImage imagedraw = image.scaled(ui->Lab_Image->width(),ui->Lab_Image->height(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->lab_fps->setText(QString("%1x%2 FPS:%3")
                             .arg(image.width())
                             .arg(image.height())
                             .arg(_fpsValue));
        _imageFlag = true;
        ui->bn_timer->setEnabled(true);
        ui->bn_photo->setEnabled(true);
        _image = QPixmap::fromImage(imagedraw);
        ui->Lab_Image->setPixmap(_image);
        _fpsCount ++;
    },Qt::AutoConnection);

    connect( _serialWidget,&SerialConnect::sendImage,this,[=](QImage image){
        _imagefullSize = QPixmap::fromImage(image);
        if(( _imagefullSize.width() == 0 )||( _imagefullSize.height() == 0 )) return;
        QImage imagedraw = image.scaled(ui->Lab_Image->width(),ui->Lab_Image->height(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->lab_fps->setText(QString("%1x%2 FPS:%3")
                             .arg(image.width())
                             .arg(image.height())
                             .arg(_fpsValue));
        _imageFlag = true;
        ui->bn_timer->setEnabled(true);
        ui->bn_photo->setEnabled(true);
        _image = QPixmap::fromImage(imagedraw);
        ui->Lab_Image->setPixmap(_image);
        _fpsCount ++;
    },Qt::AutoConnection);

    connect( _serialWidget,&SerialConnect::sendArray,this,&MainWindow::anaCMDData);
    connect( _serialWidget,&SerialConnect::sendSerialConnecting,this,[=](){
        //waitDialog.WaitMsgBos("Wait","Connect to Device\n\
        //                              Plase Wait");
        _deviceRunState = kInitDevice;
    },Qt::AutoConnection);

    connect( _httpWidget,&httpconnect::sendArray,this,&MainWindow::anaCMDData);

    ui->listWidget->clear();
    ui->listWidget->setIconSize(QSize(50,50));

    QString pathstr = QCoreApplication::applicationDirPath();
    QString setFilePath = pathstr + "/system.ini";
    _setting = new QSettings(setFilePath,QSettings::IniFormat);

    if( !_setting->contains("imagePath"))
    {
        QDir imageDir(pathstr);
        if( !imageDir.exists("image"))
        {
            imageDir.mkdir("image");
        }
        imageDir.cd("image");
        _savePath = imageDir.path();
        _setting->setValue("imagePath",_savePath);
    }
    else
    {
        _savePath = _setting->value("imagePath").toString();
    }
    readPhoto();

    if( _setting->contains("DeviceIP"))
    {
        _httpWidget->setIPAndConnect(_setting->value("DeviceIP").toString(), false);
    }

    if( _setting->contains("photoDialogState"))
    {
        _WindowsOpenFlag = _setting->value("photoDialogState").toBool();
        setMainDialogSize(_WindowsOpenFlag);
    }
    else
    {
        setMainDialogSize(false);
        _setting->setValue("photoDialogState",false);
    }

    ui->le_SavePath->setText(_savePath);

    ui->tw_connect->clear();
    ui->tw_connect->addTab(_serialWidget,"SERIAL");
    ui->tw_connect->addTab(_httpWidget,"HTTP");
    ui->tw_connect->setCurrentIndex(0);

    QListWidget *listSetial = new QListWidget(this);
    listSetial->setItemDelegate(new NoFocusFrameDelegate(this));

    ui->cb_FrameSize->setModel(listSetial->model());
    ui->cb_FrameSize->setView(listSetial);

//    ui->cb_FrameSize->addItem("UXGA(1600x1200)" ,13);
//    ui->cb_FrameSize->addItem("SXGA(1280x1024)" ,12);
//    ui->cb_FrameSize->addItem("XGA(1024x768)"   ,10);
    ui->cb_FrameSize->addItem("SVGA(800x600)"   ,9);
    ui->cb_FrameSize->addItem("VGA(640x480)"    ,8);
    ui->cb_FrameSize->addItem("CIF(400x296)"    ,6);
    ui->cb_FrameSize->addItem("QVGA(320x240)"   ,5);
    ui->cb_FrameSize->addItem("HQVGA(240x176)"  ,3);
    ui->cb_FrameSize->addItem("QQVGA(160x120)"  ,1);

    ui->comboBox_2->addItem("negative", 1);
    ui->comboBox_2->addItem("black and white", 2);
    ui->comboBox_2->addItem("greenish", 3);
    ui->comboBox_2->addItem("reddish", 4);
    ui->comboBox_2->addItem("blue", 3);
    ui->comboBox_2->addItem("retro", 3);
    ui->comboBox_2->setCurrentIndex(0);

    QListWidget *listSetial_connect = new QListWidget(this);
    listSetial_connect->setItemDelegate(new NoFocusFrameDelegate(this));
    ui->cb_connectMode->setModel(listSetial_connect->model());
    ui->cb_connectMode->setView(listSetial_connect);

    //ui->cb_connectMode->setItemIcon()
    ui->cb_connectMode->addItem(QIcon(":/icon/icon/wifi.png"), "Wifi-Http", 0);
    ui->cb_connectMode->addItem(QIcon(":/icon/icon/USB.png"), "Serial", 1);
    ui->cb_connectMode->setCurrentIndex(1);

    //ui->cb_connectMode->addItem(QIcon(":/icon/icon/timer_icon.png"), "Timer", 2);

    btnMap.insert(8,ui->bn_AWB);
    btnMap.insert(10,ui->bn_AECDsp);
    btnMap.insert(9,ui->bn_AGC);
    btnMap.insert(11,ui->bn_HMirror);
    btnMap.insert(12,ui->bn_VFlip);
    btnConnect();

    _PhotoTimer = new QTimer();

    ui->bn_timer->setEnabled(false);
    ui->bn_photo->setEnabled(false);

    connect( _PhotoTimer, &QTimer::timeout, this, [=](){
        _timerCount --;
        qInfo("SetTimer %d",_timerCount);
        ui->lb_timer->setText(QString::number(_timerCount));
        if( _timerCount <= 0 )
        {
            savePhoto();
            _timerCount = ui->le_timer->text().toInt();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendCmdToDevicd(quint8 cmd, qint16 data,quint16 retry)
{
    if( _serialWidget->getSerialState() == false )
    {
        _httpWidget->sendCMD(cmd,data);
    }
    else
    {
        _serialWidget->sendCMD(cmd,data,retry);
    }
}

void MainWindow::btnConnect()
{
    QList<quint8> keys = btnMap.keys();
    foreach( quint8 key, keys )
    {
        qInfo("set cmd %d ",key);
        btnMap[key]->setCmd(key);
        connect(btnMap[key],&clickBtn::clickedCmd,this,&MainWindow::sendBtnCmdState);
    }
}

void MainWindow::sendBtnCmdState(quint8 cmd,bool state)
{
    if( state )
    {
        sendCmdToDevicd(cmd,0x01);
        qInfo("send cmd %d - true",cmd);
    }
    else
    {
        sendCmdToDevicd(cmd,0x00);
        qInfo("send cmd %d - false",cmd);
    }
}

void MainWindow::anaCMDData(quint8 cmd,QByteArray dataArray)
{
    switch(cmd)
    {
        case 26:
            qInfo("Set Device Mode");
            sendCmdToDevicd(27,0,100);
            break;
        case 27:
            qDebug()<<"Device Mode"<<dataArray;
            changeDevicdMode(dataArray.at(0));
        break;
        case 29:
            qDebug()<<"ConfigFromDataArray"<<dataArray;
            setConfigFromDataArray(dataArray);
        break;
        case 31:
            {
                if( _deviceRunState == kInitDevice ||
                _deviceRunState == ksetDeviceToHttpMode )
                {
                    sendCmdToDevicd(32,0);
                }
            }
        break;
        case 32:
            {
                QString ssidstr(dataArray);
                qDebug()<<"WIFI SSID"<<ssidstr;
                if( dataArray.length() == 1 )
                {
                    _serialWidget->setWifiConfig();
                }
                else
                {
                    _httpWidget->setSSID(ssidstr);
                    if( _deviceRunState == kInitDevice )
                    {
                        sendCmdToDevicd(34,0,100);
                    }
                    else if( _deviceRunState == ksetDeviceToHttpMode )
                    {
                        sendCmdToDevicd(26,qint16(0),3);
                    }
                }
            }
        break;

        case 33:
            qInfo("IP:%d.%d.%d.%d",
                  quint8(dataArray.at(0)),
                  quint8(dataArray.at(1)),
                  quint8(dataArray.at(2)),
                  quint8(dataArray.at(3)));
            {
                QString ipstr = QString("%1.%2.%3.%4")
                                .arg(quint8(dataArray.at(0)))
                                .arg(quint8(dataArray.at(1)))
                                .arg(quint8(dataArray.at(2)))
                                .arg(quint8(dataArray.at(3)));

                _setting->setValue("DeviceIP",ipstr);

                if( _deviceRunState == kInitDevice ||
                    _deviceRunState == ksetDeviceToHttpMode )
                {
                    _deviceRunState = kDeviceHTTPMode;
                    _httpWidget->setIPAndConnect(ipstr);
                    ui->tw_connect->setCurrentIndex(1);
                    waitDialog.closeMsg();
                }
                else
                {
                    if( _deviceRunState == kDeviceHTTPMode )
                    {
                        IPDialog *ipmsg = new IPDialog();
                        ipmsg->ip(ipstr);
                    }
                }
            }
        break;
        case 34:
            qDebug()<<dataArray;
            if( dataArray.at(0) == 5 )
            {
                qDebug()<<"connect Successful";
                if( _deviceRunState == kInitDevice ||
                    _deviceRunState == ksetDeviceToHttpMode )
                {
                    sendCmdToDevicd(33,0);
                }
            }
            else if( dataArray.at(0) == 4 )
            {
                waitDialog.ErrorMsgBos("Error","Connect FAIL\n"
                                               "NO AP FOUND",true);

                _serialWidget->setWifiConfig();
            }
            else if( dataArray.at(0) == 3 )
            {
                waitDialog.ErrorMsgBos("Error","Connect FAIL\n"
                                               "Auth Error",true);

                _serialWidget->setWifiConfig();
            }
            else if( dataArray.at(0) == 1 )
            {
                waitDialog.ErrorMsgBos("Error","Connect FAIL\n"
                                               "Pls retry",true);

                _serialWidget->setWifiConfig();
            }
            else if( dataArray.at(0) == 0 )
            {
                sendCmdToDevicd(34,0,100);
            }
        break;
        case 37:
            _deviceRunState =kDeviceError;
        break;
    }
}

void MainWindow::setConfigFromDataArray(QByteArray data)
{
    if( data.size() == 50)
    {
        qDebug()<<"setConfigFromDataArray";

        for( int i = 0; i < 25; i++ )
        {
            qint16 data16 = data.at( i * 2 + 1 ) & 0xff;
            data16 <<= 8;
            data16 |=  data.at( i * 2 );
            bool state = ( data16 == 0x01 ) ? true : false;

            switch( i )
            {
                case 1: ui->cb_FrameSize->setCurrentIndex(ui->cb_FrameSize->findData(data16)); break;
                case 2: ui->hs_Quality->setValue(data16);   break;
                case 3: ui->hs_Contrast->setValue(data16);  break;
                case 4: ui->hs_Brightness->setValue(data16);break;
                case 5: ui->hs_Saturation->setValue(data16);break;
                case 8: ui->bn_AWB->setCheckedp(state);      break;
                case 9: ui->bn_AGC->setCheckedp(state);      break;
                case 10:break;
                case 11: ui->bn_HMirror->setCheckedp(state); break;
                case 12: ui->bn_VFlip->setCheckedp(state);   break;
                case 14: ui->bn_AGC->setCheckedp(state);     break;
                case 24: ui->hs_AELevel->setValue(data16);  break;
            }
        }
    }
}

void MainWindow::changeDevicdMode(int mode)
{
    //if( _deviceRunState == kInitDevice )
    ui->cb_connectMode->setCurrentIndex(mode);
    if( mode == 0 )
    {
        if( _deviceRunState == kInitDevice ||
            _deviceRunState == ksetDeviceToHttpMode)
        {
            sendCmdToDevicd(34,0);
        }
    }
    else
    {
        if( _deviceRunState == kInitDevice ||
            _deviceRunState == ksetDeviceToSerialMode )
        {
            _httpWidget->setConnectClose();
            ui->tw_connect->setCurrentIndex(0);
            _deviceRunState = kDeviceSerialMode;
            waitDialog.closeMsg();
            qInfo("Device is SerialMode");
        }
    }
}

QString MainWindow::saveImage(QPixmap image ,QDateTime time)
{
    QString imagepath = _savePath+"/"+time.toString("yyyy_MM_dd_hh_mm_ss_zzz")+".jpg";
    qDebug()<<imagepath;
    image.save(imagepath);
    return imagepath;
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    QPoint PressPoint;

    if( e->button() == Qt::LeftButton )
    {
        PressPoint = this->mapFromGlobal(QCursor::pos());
        if( ui->lab_title->geometry().contains(PressPoint))
        {
            _WindowsMoveFlag = true;
            m_Dragpostion = e->globalPos() - this->pos();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
    if(_WindowsMoveFlag)
    {
        move(e->globalPos() - m_Dragpostion );
    }
    m_MousePoint = e->pos();
}

void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton )
    {
        _WindowsMoveFlag = false;
    }
}

void MainWindow::on_cb_FrameSize_currentIndexChanged(int index)
{
    QVariant FrameSizeIndex = ui->cb_FrameSize->itemData(index);
    sendCmdToDevicd(0x01,quint8(FrameSizeIndex.toInt()));
    _httpWidget->sendCMD(0x01,quint8(FrameSizeIndex.toInt()));
}

void MainWindow::on_bn_photo_pressed()
{
    savePhoto();
}

void MainWindow::savePhoto()
{
    if( _imageFlag == false ) return;
    QPixmap icon = _imagefullSize.scaled(_imagefullSize.width() / 5, _imagefullSize.height() / 5, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString imagepsth = saveImage(_imagefullSize,current_date_time);
    QString time = current_date_time.toString("hh:mm:ss");
    QListWidgetItem* item = new QListWidgetItem(icon,time);
    item->setData(Qt::UserRole + 1,imagepsth);
    ui->listWidget->addItem(item);
}

void MainWindow::readPhoto()
{
    ui->listWidget->clear();
    QDir imageDir(_savePath);
    if( !imageDir.exists()) return;

    imageDir.setFilter(QDir::Files);
    QFileInfoList fileList = imageDir.entryInfoList();

    foreach(QFileInfo fileInfo, fileList)
    {
        if( fileInfo.fileName() == "." || fileInfo.fileName() == "..")continue;

        if( fileInfo.suffix() == "jpg" )
        {
            QString filename = fileInfo.fileName();
            QRegExp rx(QString("(\\d+)_(\\d+)_(\\d+)_(\\d+)_(\\d+)_(\\d+)_(\\d+)"));
            int pos = filename.indexOf(rx);
            if( pos < 0 ) continue;
            QString iconName = rx.cap(4) + ":" + rx.cap(5) + ":" + rx.cap(6);
            QPixmap photo(fileInfo.absoluteFilePath());
            QPixmap icon = photo.scaled(photo.width() / 5, photo.height() / 5, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            QListWidgetItem* item = new QListWidgetItem(icon,iconName);
            item->setData(Qt::UserRole + 1,fileInfo.absoluteFilePath());
            ui->listWidget->addItem(item);
            qDebug()<<fileInfo.absoluteFilePath()<<iconName;
        }
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ImageDialog* dialog = new ImageDialog(this);
    QString filePath = item->data(Qt::UserRole + 1).toString();
    QPixmap image(filePath);
    qInfo("%d,%d",image.width(),image.height());
    dialog->showImage(image);
    dialog->deleteLater();
}
/*
void MainWindow::on_bn_h_mirror_clicked(bool checked)
{
    if( checked )
    {
        sendCmdToDevicd(11,0x01);
    }
    else
    {
        sendCmdToDevicd(11,0x00);
    }
}

void MainWindow::on_bn_v_flip_clicked(bool checked)
{
    if( checked )
    {
        sendCmdToDevicd(12,0x01);
    }
    else
    {
        sendCmdToDevicd(12,0x00);
    }
}
*/

void MainWindow::on_bn_savePath_pressed()
{
    QString path = QFileDialog::getExistingDirectory(this,"save",_savePath);
    if( path.isEmpty()) return;

    _savePath = path;
    ui->le_SavePath->setText(_savePath);
    _setting->setValue("imagePath",_savePath);
}

void MainWindow::on_hs_Brightness_valueChanged(int value)
{
//    sendCmdToDevicd(4,qint16(value));
}

void MainWindow::on_hs_Quality_valueChanged(int value)
{
//    sendCmdToDevicd(2,qint16(value));
}

void MainWindow::on_hs_Contrast_valueChanged(int value)
{
//    sendCmdToDevicd(3,qint16(value));
}

void MainWindow::on_hs_Saturation_valueChanged(int value)
{
//    sendCmdToDevicd(5,qint16(value));
}

void MainWindow::on_hs_AELevel_valueChanged(int value)
{
//    sendCmdToDevicd(24,qint16(value));
}

void MainWindow::on_hs_Gain_valueChanged(int value)
{
//    sendCmdToDevicd(6,qint16(value));
}
void MainWindow::setMainDialogSize(bool mode)
{
    int width = 980;
    int titleWidth = 950;
    int posExitBtn = 950;
    if( mode == true )
    {
        width = 1230;
        posExitBtn = titleWidth = 1200;
    }
    else
    {
        width = 980;
        posExitBtn = titleWidth = 950;
    }
    ui->Bn_Exit->move(posExitBtn,0);
    ui->lab_title->setGeometry(0,0,titleWidth,30);
    ui->bn_openOther->setProperty("isopen",_WindowsOpenFlag);
    ui->bn_openOther->style()->unpolish(ui->bn_openOther);
    this->resize(width,this->height());
}
void MainWindow::on_bn_openOther_pressed()
{
    _WindowsOpenFlag = !_WindowsOpenFlag;
    setMainDialogSize(_WindowsOpenFlag);
    _setting->setValue("photoDialogState",_WindowsOpenFlag);
}

void MainWindow::on_cb_connectMode_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if( _deviceRunState == kInitDevice ) return;

    int mode = ui->cb_connectMode->currentData().toInt();
    qInfo("Change Mode %d",mode);
    if( index == 0 )
    {
        _deviceRunState = ksetDeviceToHttpMode;
        sendCmdToDevicd(32,qint16(mode),3);
        waitDialog.WaitMsgBos("Wait","Set to Http Mode\n\
                                      Plase Wait");
    }
    else
    {
        _deviceRunState = ksetDeviceToSerialMode;
        sendCmdToDevicd(26,qint16(mode),3);
        waitDialog.WaitMsgBos("Wait","Set to Serial Mode\n\
                                      Plase Wait");
        //_httpWidget->setConnectClose();
        //ui->tw_connect->setCurrentIndex(0);
        //_deviceRunState = kDeviceSerialMode;
    }
    //sendCmdToDevicd(26,qint16(mode),1);

    //ui->tw_connect->setCurrentIndex(mode);
    //ui->lab_connectMode->setProperty("mode",mode);
    //ui->lab_connectMode->style()->unpolish(ui->lab_connectMode);
    //ui->lab_connectMode->repaint();
}

void MainWindow::on_bn_photo_3_pressed()
{
    sendCmdToDevicd(28,0);
}

void MainWindow::on_le_timer_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    ui->lb_timer->setText(arg1);
    _PhotoTimer->stop();
    ui->bn_timer->setChecked(false);
}

void MainWindow::on_bn_timer_pressed()
{

}

void MainWindow::on_Bn_Exit_pressed()
{
    QApplication* app = nullptr;
    app->quit();
}

void MainWindow::on_bn_timer_clicked(bool checked)
{
    if( checked )
    {
        _timerCount = ui->le_timer->text().toInt();
        if( _timerCount <= 0 )
        {
            _timerCount = 1;
            ui->lb_timer->setText(QString::number(_timerCount));
            ui->le_timer->setText(QString::number(_timerCount));
        }
        qInfo("SetTimer to %d",_timerCount);
        ui->lb_timer->setText(QString::number(_timerCount));
        _PhotoTimer->start(1000);
    }
    else
    {
        _PhotoTimer->stop();
    }
}

void MainWindow::on_hs_Quality_sliderReleased()
{
    sendCmdToDevicd(2,qint16(ui->hs_Quality->value()));
}

void MainWindow::on_hs_Brightness_sliderReleased()
{
    sendCmdToDevicd(4,qint16(ui->hs_Brightness->value()));
}

void MainWindow::on_hs_Contrast_sliderReleased()
{
    sendCmdToDevicd(3, qint16(ui->hs_Contrast->value()));
}

void MainWindow::on_hs_Saturation_sliderReleased()
{
    sendCmdToDevicd(5, qint16(ui->hs_Saturation->value()));
}

void MainWindow::on_hs_AELevel_sliderReleased()
{
    sendCmdToDevicd(24, qint16(ui->hs_AELevel->value()));
}

void MainWindow::on_hs_Gain_sliderReleased()
{
    sendCmdToDevicd(6, qint16(ui->hs_Gain->value()));
}

void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    sendCmdToDevicd(22, qint16(index));
}

void MainWindow::on_bn_photo_3_clicked()
{

}
