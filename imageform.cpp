#include "imageform.h"
#include "ui_imageform.h"

ImageForm::ImageForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageForm)
{
    ui->setupUi(this);
    _imageLable = new QLabel(this);
    _imageLable->setGeometry(this->geometry());

    QPixmap pixmap(_imageLable->width(),_imageLable->height());
    QPainter painter(&pixmap);
    painter.setBrush(QBrush(QColor(255,0,0)));
    painter.drawRect(0,0,_imageLable->width(),_imageLable->height());
    _imageLable->setPixmap(pixmap);
}

ImageForm::~ImageForm()
{
    delete ui;
}
