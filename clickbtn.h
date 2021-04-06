#ifndef CLICKBTN_H
#define CLICKBTN_H

#include <QObject>
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include <QFont>

class clickBtn : public QPushButton
{
    Q_OBJECT
public:
    explicit clickBtn(QWidget *parent = nullptr);
    void setCheckedp(bool clicked);
    void setCmd(quint8 cmd){ _cmd = cmd;}

signals:
    void clicked(bool);
    void clickedCmd(quint8,bool);
public slots:

private:
    bool _clicked = false;
    QTimer* _amaTimer;
    QPointF pNow,pStart,pEnd,pLeft,pRight;
    int   alphaStart,alphaEnd,alphaNow;
    int   count = 0;
    quint8   _cmd;
protected:
    void paintEvent(QPaintEvent *) override;
    QSize	sizeHint() const override;
    void resizeEvent(QResizeEvent*)override;

};

#endif // CLICKBTN_H
