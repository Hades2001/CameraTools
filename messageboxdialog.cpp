#include "messageboxdialog.h"
#include "ui_messageboxdialog.h"

MessageBoxDialog::MessageBoxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBoxDialog)
{
    ui->setupUi(this);
}

MessageBoxDialog::~MessageBoxDialog()
{
    delete ui;
}

void MessageBoxDialog::WarringMsgBos(QString title,
                   QString text, bool modal)
{
    this->setWindowTitle(title);
    ui->le_text->setText(text);
    ui->le_icon->setPixmap(QPixmap(":/icon/icon/Warring.png"));
    this->setModal(modal);
    if( modal )
    {
        this->exec();
    }
    else
    {
        this->show();
    }
}

void MessageBoxDialog::ErrorMsgBos(QString title,
                 QString text, bool modal)
{
    this->setWindowTitle(title);
    ui->le_text->setText(text);
    ui->le_icon->setPixmap(QPixmap(":/icon/icon/error.png"));
    this->setModal(modal);
    if( modal )
    {
        this->exec();
    }
    else
    {
        this->show();
    }
}

void MessageBoxDialog::WaitMsgBos(QString title,
                QString text, bool modal)
{
    this->setWindowTitle(title);
    ui->le_text->setText(text);

    QMovie *pMovie = new QMovie(":/icon/icon/loading (1).gif");
    ui->le_icon->setMovie(pMovie);
    pMovie->start();
    this->setModal(modal);
    if( modal )
    {
        this->exec();
    }
    else
    {
        this->show();
    }
}

void MessageBoxDialog::MsgBos(QString title,
            QString text,
            QPixmap icon,
            bool modal )
{
    this->setWindowTitle(title);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    ui->le_text->setText(text);
    ui->le_icon->setPixmap(icon);
    this->setModal(modal);
    if( modal )
    {
        this->exec();
    }
    else
    {
        this->show();
    }
}
