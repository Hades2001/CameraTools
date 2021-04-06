#ifndef IMAGEFORM_H
#define IMAGEFORM_H

#include <QWidget>
#include <QLabel>
#include <QPainter>

namespace Ui {
class ImageForm;
}

class ImageForm : public QWidget
{
    Q_OBJECT

public:
    explicit ImageForm(QWidget *parent = nullptr);
    ~ImageForm();
    void drawImage(QPixmap*);

private:
    Ui::ImageForm *ui;

private:
    QLabel *_imageLable;
};

#endif // IMAGEFORM_H
