#ifndef MESSAGEBOXDIALOG_H
#define MESSAGEBOXDIALOG_H

#include <QDialog>
#include <QMovie>

namespace Ui {
class MessageBoxDialog;
}

class MessageBoxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageBoxDialog(QWidget *parent = nullptr);
    ~MessageBoxDialog();

    void WarringMsgBos(QString title,
                       QString text,
                       bool modal = false);

    void ErrorMsgBos(QString title,
                     QString text,
                     bool modal = false);

    void WaitMsgBos(QString title,
                    QString text,
                    bool modal = false);

    void MsgBos(QString title,
                QString text,
                QPixmap icon,
                bool modal = false);

    void closeMsg(){ this->close();}


private:
    Ui::MessageBoxDialog *ui;
};

#endif // MESSAGEBOXDIALOG_H
