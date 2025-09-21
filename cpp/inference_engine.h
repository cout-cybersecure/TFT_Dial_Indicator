#pragma once
#include <deque>
#pragma push_macro("slots")
#pragma push_macro("signals")
#ifdef slots
#undef slots
#endif
#ifdef signals
#undef signals
#endif
#include <torch/script.h>
#pragma pop_macro("signals")
#pragma pop_macro("slots")
#include <QObject>

class InferenceEngine:public QObject
{
    Q_OBJECT
public:
    explicit InferenceEngine(QObject* parent=nullptr);
    bool load(const QString& path);
public slots:
    void on_tick(qint64 ts_ms,double price);
signals:
    void score(double s);
private:
    void push_price(qint64 ts_ms,double p);
    double compute_score();
    double std_window(int idx,int n) const;
    double r1_at(int idx) const;
    double rN_at(int idx,int n) const;
    torch::jit::script::Module model;
    bool have_model=false;
    std::deque<double> prices;
    std::deque<qint64> times;
    std::deque<double> ret_win;
    int ret_cap=1200;
    int L=20;
    double calib_scale=1e-6;
    bool calib_frozen=false;
    int ticks_since_scale=0;
    double ema_pred_mean=0.0;
    double ema_pred_alpha=0.05;
    double prev_score=50.0;
};
