#include "imagedialog.h"
#include "ui_imagedialog.h"

ImageDialog::ImageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageDialog)
{
    ui->setupUi(this);
}

ImageDialog::~ImageDialog()
{
    delete ui;
}

void ImageDialog::showImage(QPixmap image)
{
    this->resize(image.width(),image.height());
    qInfo("%d,%d",image.width(),image.height());
    ui->lab_image->setPixmap(image);
    this->exec();
}
