#include "burner.h"
#include "ui_burner.h"

Burner::Burner(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Burner)
{
    ui->setupUi(this);

    _esptool = new QProcess(this);

    connect(_esptool,&QProcess::readyReadStandardOutput,this,[=](){
        QString outstr = _esptool->readAllStandardOutput().data();
        qDebug()<<outstr;
        ui->te_out->append(outstr);

        //Writing at 0x0007d000... (96 %)
        QRegExp rx(QString("Writing at 0x(.*)\\.\\.\\.\\s\\((\\d+)\\s\\%\\)"));
        int pos = outstr.indexOf(rx);
        qDebug()<<pos;
        if(pos>=0)
        {
            emit WriteValue(rx.cap(2).toInt());
            //qDebug()<<rx.cap(2);
        }
    });

    connect(_esptool,&QProcess::readyReadStandardError,this,[=](){

        QString outstr = _esptool->readAllStandardError().data();
        qDebug()<<outstr;
        ui->te_out->append(outstr);
    });

    connect(_esptool, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){
        Q_UNUSED(exitStatus)
        qInfo("exitCode %d",exitCode);
        if( exitCode == 0 )
        {
            this->close();
        }
    });
}

Burner::~Burner()
{
    delete ui;
}

void Burner::startEsptool(QStringList cmd)
{
    QString pathstr = QCoreApplication::applicationDirPath();
    pathstr.append("/esptool.exe");
    _esptool->start(pathstr,cmd);
    _esptool->waitForStarted();

    //this->exec();
}

void Burner::on_te_out_textChanged()
{
    ui->te_out->moveCursor(QTextCursor::End);
}
