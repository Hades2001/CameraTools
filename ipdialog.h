#ifndef IPDIALOG_H
#define IPDIALOG_H

#include <QDialog>
#include <QClipboard>
#include <QMessageBox>

namespace Ui {
class IPDialog;
}

class IPDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IPDialog(QWidget *parent = nullptr);
    ~IPDialog();
public:
    void ip(QString);

private slots:
    void on_bn_ok_pressed();
    void on_bn_copy_pressed();

private:
    Ui::IPDialog *ui;
};

#endif // IPDIALOG_H
