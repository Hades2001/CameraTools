#include "clickbtn.h"

clickBtn::clickBtn(QWidget *parent) : QPushButton(parent)
{
    _amaTimer = new QTimer();
    pNow = pLeft;
    alphaNow = 0;

    this->setCheckable(true);
    this->setChecked(false);

    connect(this,&QPushButton::clicked,this,[=](bool checked){
        Q_UNUSED(checked)
        _clicked = !_clicked;
        if( _clicked )
        {
            pEnd = pRight;
            pStart = pNow;
            alphaStart = alphaNow;
            alphaEnd = 255;
        }
        else
        {
            pEnd = pLeft;
            pStart = pNow;
            alphaStart = alphaNow;
            alphaEnd = 0;
        }
        emit clicked(_clicked);
        emit clickedCmd(_cmd,_clicked);
        _amaTimer->start(9);
        count = 0;
    });

    connect(_amaTimer,&QTimer::timeout,this,[=](){
        pNow.setX( pNow.x() + (( pEnd.x() - pStart.x()) / 10 ));
        pNow.setY( pLeft.y());

        alphaNow = ( alphaNow + ( alphaEnd - alphaStart ) / 10 );
        count ++;
        if(count == 10 )
        {
            count = 0;
            _amaTimer->stop();
            if( _clicked )
            {
                pNow = pRight;
                alphaNow = 255;
            }
            else
            {
                pNow = pLeft;
                alphaNow = 0;
            }
        }
        this->repaint();
    });
}
void clickBtn::setCheckedp(bool clicked)
{
    _clicked = clicked;
    if( _clicked )
    {
        pEnd = pRight;
        pStart = pNow;
        alphaStart = alphaNow;
        alphaEnd = 255;
    }
    else
    {
        pEnd = pLeft;
        pStart = pNow;
        alphaStart = alphaNow;
        alphaEnd = 0;
    }
    _amaTimer->start(9);
    count = 0;
}
void clickBtn::resizeEvent(QResizeEvent*)
{
    this->resize(this->height()*5,this->height());
    pLeft  = QPointF(this->height()*( 0.5 ) ,this->height()/2);
    pRight = QPointF(this->height()*( 0.5 + 0.55 ) ,this->height()/2);
    if( _clicked )
    {
        pNow = pRight;
        alphaNow = 255;
    }
    else
    {
        pNow = pLeft;
        alphaNow = 0;
    }
    this->repaint();
}

QSize clickBtn::sizeHint() const
{
    QSize s = QPushButton::sizeHint();
    s.setWidth(s.height()*5);
    return s;
}

void clickBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    pLeft  = QPointF(this->height()*( 0.5 ) ,this->height()/2);
    pRight = QPointF(this->height()*( 0.5 + 0.55 ) ,this->height()/2);
    /*********************************************************
     *                          |
     *                          |
     *                          |       ON/OFF
     *                          |
     *                          |
     ********************************************************/
    //
    QRectF BtnRect;
    QRectF TextRect;
    QColor BkColor;
    qreal radius = this->height()*0.45;
    QColor EllipseColor(240,240,240);
    qreal EllipseR = this->height()*0.375;

    BkColor = QColor(0, 173, 184,alphaNow);

    BtnRect.setX(this->height()*0.05);
    BtnRect.setY(this->height()*0.05);
    BtnRect.setWidth(this->height()*1.45);
    BtnRect.setHeight(this->height()*0.9);

    TextRect.setX( this->height()*1.6 );
    TextRect.setY( 0 );
    TextRect.setWidth( this->height()*1.6 );
    TextRect.setHeight( this->height() );

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(QColor(173, 173, 173),0));
    painter.setBrush(QBrush(QColor(173, 173, 173)));
    painter.drawRoundedRect(BtnRect,radius,radius);

    painter.setPen(QPen(QBrush(BkColor),0));
    painter.setBrush(QBrush(BkColor));
    painter.drawRoundedRect(BtnRect,radius,radius);

    painter.setPen(QPen(QBrush(EllipseColor),0));
    painter.setBrush(QBrush(EllipseColor));
    painter.drawEllipse(pNow,EllipseR,EllipseR);

    QFont font = painter.font();
    font.setFamily("Bahnschrift Light");
    font.setPixelSize(int(this->height()*0.7));
    painter.setPen(QPen(QColor(255,255,255),3));
    painter.setFont(font);

    painter.setPen(QPen(QColor(255,255,255,alphaNow),3));
    painter.drawText(TextRect,Qt::AlignVCenter | Qt::AlignHCenter,"ON");
    painter.setPen(QPen(QColor(255,255,255,255-alphaNow),3));
    painter.drawText(TextRect,Qt::AlignVCenter | Qt::AlignHCenter,"OFF");

    //QFont font("Bahnschrift Light");
    //painter.drawRect(QRect(0,0,this->width(),this->height()));
}
