#include "dial_widget.h"
#include <QPainter>
#include <QtMath>
#include <algorithm>

static inline QPointF polar(const QPointF& c,qreal r,qreal deg)
{
    qreal a=qDegreesToRadians(deg);
    return QPointF(c.x()+r*qCos(a),c.y()+r*qSin(a));
}

static void draw_arrow(QPainter& p,const QPointF& tail,const QPointF& tip,qreal width)
{
    QPen shaft(p.pen());
    shaft.setWidthF(width);
    p.setPen(shaft);
    qreal ang=std::atan2(tip.y()-tail.y(),tip.x()-tail.x());
    qreal head=std::max<qreal>(width*2.6,14.0);
    qreal half=head*0.55;
    QPointF back(tip.x()-head*std::cos(ang),tip.y()-head*std::sin(ang));
    p.drawLine(tail,back);
    QPointF l(back.x()+half*std::sin(ang),back.y()-half*std::cos(ang));
    QPointF r(back.x()-half*std::sin(ang),back.y()+half*std::cos(ang));
    QBrush prevBrush=p.brush();
    QPen prevPen=p.pen();
    p.setBrush(shaft.color());
    p.setPen(Qt::NoPen);
    QPolygonF poly; poly<<tip<<l<<r;
    p.drawPolygon(poly);
    p.setPen(prevPen);
    p.setBrush(prevBrush);
}

DialWidget::DialWidget(QWidget* parent):QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

QSize DialWidget::minimumSizeHint() const
{
    return QSize(360,220);
}

void DialWidget::set_min_max(double min_v,double max_v)
{
    if(max_v<=min_v) return;
    m_min=min_v;
    m_max=max_v;
    if(m_value<m_min) m_value=m_min;
    if(m_value>m_max) m_value=m_max;
    update();
}

void DialWidget::set_value(double v)
{
    if(v<m_min) v=m_min;
    if(v>m_max) v=m_max;
    m_value=v;
    update();
}

void DialWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    int w=width();
    int h=height();
    qreal topMargin=std::max<qreal>(72.0,h*0.16);
    qreal availH=std::max<qreal>(1.0,h-topMargin);
    qreal D=std::min<double>(w*0.86,2.0*availH*0.92);
    qreal thick=std::max<qreal>(6.0,D*0.085);
    QRectF arcRect((w-D)/2.0,topMargin+(availH*0.95-D/2.0),D,D);
    QRectF ringRect=arcRect.adjusted(thick/2.0,thick/2.0,-thick/2.0,-thick/2.0);
    QPointF c=arcRect.center();
    qreal rOuter=ringRect.width()/2.0;
    qreal rInner=rOuter-thick*0.55;
    QPen basePen(QColor(238,241,246),thick,Qt::SolidLine,Qt::FlatCap);
    p.setPen(basePen);
    p.drawArc(ringRect,180*16,-180*16);
    int steps=160;
    for(int i=0;i<steps;++i)
    {
        qreal t0=qreal(i)/steps;
        qreal t1=qreal(i+1)/steps;
        auto lerp=[&](int a,int b,qreal t){ return int(std::round(a+(b-a)*t)); };
        QColor c0,c1;
        if(t0<0.5)
        {
            qreal u=t0/0.5;
            c0=QColor(255,48,38);
            QColor y(255,204,0);
            c0.setRed(lerp(c0.red(),y.red(),u));
            c0.setGreen(lerp(c0.green(),y.green(),u));
            c0.setBlue(lerp(c0.blue(),y.blue(),u));
        }
        else
        {
            qreal u=(t0-0.5)/0.5;
            QColor y(255,204,0);
            QColor g(40,200,90);
            c0.setRed(lerp(y.red(),g.red(),u));
            c0.setGreen(lerp(y.green(),g.green(),u));
            c0.setBlue(lerp(y.blue(),g.blue(),u));
        }
        if(t1<0.5)
        {
            qreal u=t1/0.5;
            c1=QColor(255,48,38);
            QColor y(255,204,0);
            c1.setRed(lerp(c1.red(),y.red(),u));
            c1.setGreen(lerp(c1.green(),y.green(),u));
            c1.setBlue(lerp(c1.blue(),y.blue(),u));
        }
        else
        {
            qreal u=(t1-0.5)/0.5;
            QColor y(255,204,0);
            QColor g(40,200,90);
            c1.setRed(lerp(y.red(),g.red(),u));
            c1.setGreen(lerp(y.green(),g.green(),u));
            c1.setBlue(lerp(y.blue(),g.blue(),u));
        }
        qreal a0=180.0-180.0*t0;
        qreal a1=180.0-180.0*t1;
        int start16=int(std::round(a0*16.0));
        int span16=int(std::round((a1-a0)*16.0));
        QPen segPen(c0,thick,Qt::SolidLine,Qt::FlatCap);
        p.setPen(segPen);
        p.drawArc(ringRect,start16,span16);
    }
    QPen divPen(QColor(30,30,30,180),std::max<qreal>(2.0,thick*0.12),Qt::SolidLine,Qt::FlatCap);
    p.setPen(divPen);
    p.drawLine(polar(c,rInner,120),polar(c,rOuter,120));
    p.drawLine(polar(c,rInner,60),polar(c,rOuter,60));
    QFont f=p.font();
    f.setBold(true);
    f.setPointSizeF(std::max<qreal>(12.0,D*0.035));
    p.setFont(f);
    QFontMetrics fm(f);
    qreal labelY=ringRect.top()-thick*0.55;
    QColor textClr(25,25,25);
    QColor arrowClr(25,25,25);
    qreal arrowW=std::max<qreal>(2.0,thick*0.08);
    qreal pad=std::max<qreal>(8.0,thick*0.35);
    {
        QString text=QStringLiteral("Sell");
        int tw=fm.horizontalAdvance(text);
        int th=fm.height();
        qreal cx=w*0.25;
        qreal x=std::clamp(cx-tw*0.5,10.0,w*0.5-tw-20.0);
        QRectF rect(x,labelY-th,tw,th);
        p.setPen(QPen(arrowClr,arrowW,Qt::SolidLine,Qt::RoundCap));
        qreal L=std::max<qreal>(34.0,tw*0.9);
        QPointF tail(rect.left()-pad,rect.center().y());
        QPointF tip(tail.x()-L,tail.y());
        draw_arrow(p,tail,tip,arrowW);
        p.setPen(textClr);
        p.drawText(rect,Qt::AlignCenter,text);
    }
    {
        QString text=QStringLiteral("Buy");
        int tw=fm.horizontalAdvance(text);
        int th=fm.height();
        qreal cx=w*0.75;
        qreal x=std::clamp(cx-tw*0.5,w*0.5+20.0,w-tw-10.0);
        QRectF rect(x,labelY-th,tw,th);
        p.setPen(QPen(arrowClr,arrowW,Qt::SolidLine,Qt::RoundCap));
        qreal L=std::max<qreal>(34.0,tw*0.9);
        QPointF tail(rect.right()+pad,rect.center().y());
        QPointF tip(tail.x()+L,tail.y());
        draw_arrow(p,tail,tip,arrowW);
        p.setPen(textClr);
        p.drawText(rect,Qt::AlignCenter,text);
    }
    double t=(m_value-m_min)/(m_max-m_min);
    qreal ang=180.0+180.0*qBound(0.0,t,1.0);
    QPen needlePen(QColor(18,18,18),std::max<qreal>(3.0,thick*0.12),Qt::SolidLine,Qt::RoundCap);
    p.setPen(needlePen);
    p.drawLine(c,polar(c,rInner+thick*0.45,ang));
    p.setBrush(QColor(35,35,35));
    p.setPen(Qt::NoPen);
    p.drawEllipse(c,thick*0.22,thick*0.22);
}

void DialWidget::resizeEvent(QResizeEvent*)
{
    update();
}
