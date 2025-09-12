#pragma once
#include <QWidget>

class DialWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DialWidget(QWidget* parent = nullptr);
    QSize minimumSizeHint() const override;
    void setValue(double v);
    void setMinMax(double minV, double maxV);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    double m_min = 0.0;
    double m_max = 100.0;
    double m_value = 0.0;
};
