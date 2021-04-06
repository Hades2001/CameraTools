#include "serialconnect.h"
#include "ui_serialconnect.h"

static int JPEGCorruptFlag;

void JPEGMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    Q_UNUSED(type)
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    JPEGCorruptFlag = 0;

    if(( type == QtWarningMsg )&&(msg.indexOf("premature") != -1))
    {
        JPEGCorruptFlag = -1;
    }

    mutex.unlock();
}

SerialConnect::SerialConnect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialConnect)
{
    ui->setupUi(this);

    QListWidget *listSetial = new QListWidget(this);
    listSetial->setItemDelegate(new NoFocusFrameDelegate(this));

    ui->cb_serial->setModel(listSetial->model());
    ui->cb_serial->setView(listSetial);

    connect(ui->cb_serial,&ClickedComboBox::clicked,[=](bool){
        scanSerialName();
    });

    scanSerialName();

    _esptool = new QProcess(this);

    connect(_esptool,&QProcess::readyReadStandardOutput,this,[=](){
        QString outstr = _esptool->readAllStandardOutput().data();
        qDebug()<<outstr;
        QRegExp rx(QString("Writing at 0x(.*)\\.\\.\\.\\s\\((\\d+)\\s\\%\\)"));
        int pos = outstr.indexOf(rx);
        qDebug()<<pos;
        if(pos>=0)
        {
            _burner = kBurnerWriting;
            _burnerTimer->stop();
            ui->bn_connect_3->setText(QString("Writing\n%1%").arg(rx.cap(2).toInt()));
            qDebug()<<rx.cap(2);
        }
    });

    connect(_esptool,&QProcess::readyReadStandardError,this,[=](){

        QString outstr = _esptool->readAllStandardError().data();
        QMessageBox *infoBox = new QMessageBox(
                                QMessageBox::NoIcon,
                                "burner",outstr,
                                QMessageBox::Close, nullptr);
        infoBox->setStyleSheet("background-color:white");
        infoBox->show();
        qDebug()<<outstr;
        _burner = kBurnerIDLE;
    });

    connect(_esptool, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){
        Q_UNUSED(exitStatus)
        qInfo("exitCode %d",exitCode);
        if( exitCode == 0 )
        {
            QMessageBox *infoBox = new QMessageBox(
                                    QMessageBox::NoIcon,
                                    "burner","Download successful",
                                    QMessageBox::Close, nullptr);
            infoBox->setStyleSheet("background-color:white");
            infoBox->show();
            //infoBox->information(this,"burner","Download successful");
        }
        else
        {
            QMessageBox *infoBox = new QMessageBox(
                                    QMessageBox::NoIcon,
                                    "burner","Download faild",
                                    QMessageBox::Close, nullptr);
            infoBox->setStyleSheet("background-color:white");
            infoBox->show();
        }
        _burner = kBurnerIDLE;
        _burnerTimer->stop();
        ui->bn_connect_3->setText("burner");
    });

    _burnerTimer = new QTimer(this);
    connect(_burnerTimer,&QTimer::timeout,this,[=](){
        //if( _burner == kBurnerWait )
        {
            _burnerCount++;
            QString ch('-');
            switch(_burnerCount%4)
            {
                case 0:ch = '-';break;
                case 1:ch = '\\';break;
                case 2:ch = '|';break;
                case 3:ch = '/';break;
                default:ch = '-';break;
            }
            ui->bn_connect_3->setText("Wating\n" + ch);
        }
    });

    _serialTh = new SerialRevice();
    connect(this,&SerialConnect::SerialCtrl,_serialTh,&SerialRevice::SerialCtrl);
    connect(this,&SerialConnect::sendDataToDevice,_serialTh,&SerialRevice::sendData);
    connect(this,&SerialConnect::sendArrayToDevice,_serialTh,&SerialRevice::sendArray);
    connect(_serialTh,&SerialRevice::revicePack,this,&SerialConnect::revicePack);

    connect(_serialTh,&SerialRevice::serialState,this,[=](int state){
        if( state == SerialRevice::kSerialOpen)
        {
            _serialState = SerialRevice::kSerialOpen;
            ui->bn_connect->setText("close");
            _isConnected = true;
            emit sendSerialConnecting();
            emit sendDataToDevice(29,0);
            emit sendDataToDevice(27,0);
        }
        else
        {
            _serialState = SerialRevice::kSerialClose;
            ui->bn_connect->setText("connect");
            _isConnected = false;
        }
    });
    connect(_serialTh,&SerialRevice::serialError,this,[=](){
        ui->bn_connect->setText("connect");
        _isConnected = false;
        _serialState = SerialRevice::kSerialClose;
        MessageBoxDialog *errorBox = new MessageBoxDialog;
        errorBox->setAttribute(Qt::WA_DeleteOnClose);
        errorBox->ErrorMsgBos("Serial Error","Serial Error \n Error Code 000",true);
    });
}

SerialConnect::~SerialConnect()
{
    delete ui;
}

void SerialConnect::revicePack(quint8 cmd,QByteArray dataArray)
{
    if( cmd == 0x00 )
    {
        QImage image;
        JPEGCorruptFlag = 0;
        qInstallMessageHandler(JPEGMessageOutput);
        image.loadFromData(dataArray);
        qInstallMessageHandler(nullptr);
        if( JPEGCorruptFlag == 0 )
        {
            emit sendImage(image);
        }
    }
    else
    {
        emit sendArray(cmd,dataArray);
    }
}

void SerialConnect::scanSerialName()
{
    ui->cb_serial->clear();
    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
        ui->cb_serial->addItem("无串口");
        return;
    }
    foreach (QSerialPortInfo info, infos)
    {
        ui->cb_serial->addItem(info.portName());
        ui->cb_serial->setCurrentIndex(0);
    }
}
void SerialConnect::sendSerialCtrl(int state)
{
    emit SerialCtrl(ui->cb_serial->currentText(),state);
}
void SerialConnect::on_bn_connect_pressed()
{
    if( _serialState == SerialRevice::kSerialOpen )
    {
        emit SerialCtrl(ui->cb_serial->currentText(),SerialRevice::kSerialClose);
    }
    else
    {
        emit SerialCtrl(ui->cb_serial->currentText(),SerialRevice::kSerialOpen);
    }
}

void SerialConnect::on_bn_connect_3_pressed()
{
    //if( _serial->isOpen())
    //{
    //    _serial->close();
    //    _sendTimer->stop();
    //    ui->bn_connect->setText("connect");
    //}

    QString pathstr = QCoreApplication::applicationDirPath();

    QStringList arg;
    arg.append("-p");
    arg.append(ui->cb_serial->currentText());
    arg.append("-b 1500000");
    arg.append("write_flash");
    arg.append("0x1000");
    arg.append(pathstr + "/firmware.bin");
    qDebug()<<arg;

    _esptool->start(pathstr+"/esptool.exe",arg);
    _esptool->waitForStarted();

    _burnerTimer->start(500);
    _burnerCount = 0;
    _burner = kBurnerWait;

    //ui->bn_connect_3->setText("Writing");
    //esptool->startEsptool(arg);
}
void SerialConnect::setWifiConfig()
{
    wifiConfigDialog *dialog = new wifiConfigDialog();
    QByteArray wificonfig = dialog->getWifiConfigArray();
    emit sendArrayToDevice(31,wificonfig);
}
void SerialConnect::on_bn_setting_pressed()
{
    wifiConfigDialog *dialog = new wifiConfigDialog();
    QByteArray wificonfig = dialog->getWifiConfigArray();
    emit sendArrayToDevice(31,wificonfig);
    emit sendDataToDevice(32,0);
}

void SerialConnect::on_bn_getIP_pressed()
{
    emit sendDataToDevice(34,0);
    //emit sendDataToDevice(33,0);
}

void SerialConnect::on_bn_connect_clicked()
{

}
