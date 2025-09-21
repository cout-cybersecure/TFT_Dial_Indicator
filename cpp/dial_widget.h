#pragma once
#include <QWidget>

class DialWidget:public QWidget
{
    Q_OBJECT
public:
    explicit DialWidget(QWidget* parent=nullptr);
    QSize minimumSizeHint() const override;
    void set_value(double v);
    void set_min_max(double min_v,double max_v);
    void setValue(double v){ set_value(v); }
    void setMinMax(double a,double b){ set_min_max(a,b); }
protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
private:
    double m_min=0.0;
    double m_max=100.0;
    double m_value=50.0;
};
