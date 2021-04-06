#ifndef KEYDEFAULT_H
#define KEYDEFAULT_H

#include <QWidget>

namespace Ui {
class keyDefault;
}

class keyDefault : public QWidget
{
    Q_OBJECT

public:
    explicit keyDefault(QWidget *parent = nullptr);
    ~keyDefault();

private:
    Ui::keyDefault *ui;
};

#endif // KEYDEFAULT_H
