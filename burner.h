#ifndef BURNER_H
#define BURNER_H

#include <QDialog>
#include <QProcess>
#include <QDebug>

namespace Ui {
class Burner;
}

class Burner : public QDialog
{
    Q_OBJECT

public:
    explicit Burner(QWidget *parent = nullptr);
    ~Burner();
    void startEsptool(QStringList cmd);

private:
    Ui::Burner *ui;
    QProcess *_esptool;

signals:
    void WriteValue(int);

private slots:
    void on_te_out_textChanged();
};

#endif // BURNER_H
