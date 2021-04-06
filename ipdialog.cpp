#include "ipdialog.h"
#include "ui_ipdialog.h"

IPDialog::IPDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IPDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Device IP");
}

IPDialog::~IPDialog()
{
    delete ui;
}
void IPDialog::ip(QString ipstr)
{
    ui->le_ip->setText(ipstr);
    this->exec();
}
void IPDialog::on_bn_ok_pressed()
{
    this->close();
}

void IPDialog::on_bn_copy_pressed()
{
    QClipboard *clip=QApplication::clipboard();
    clip->setText(ui->le_ip->text());
}
