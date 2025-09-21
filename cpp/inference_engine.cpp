#include "inference_engine.h"
#include <QDateTime>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>

static inline double clamp01(double x)
{
    if(x<0.0) return 0.0;
    if(x>1.0) return 1.0;
    return x;
}

InferenceEngine::InferenceEngine(QObject* parent):QObject(parent)
{
}

bool InferenceEngine::load(const QString& path)
{
    try
    {
        std::string p(path.toUtf8().constData());
        model=torch::jit::load(p,torch::kCPU);
        model.eval();
        have_model=true;
        return true;
    }
    catch(...)
    {
        have_model=false;
        return false;
    }
}

void InferenceEngine::push_price(qint64 ts_ms,double p)
{
    prices.push_back(p);
    times.push_back(ts_ms);
    if(prices.size()>8192) prices.pop_front();
    if(times.size()>8192) times.pop_front();
    int n=int(prices.size());
    if(n>=2)
    {
        double r=std::log(prices[n-1]/prices[n-2]);
        ret_win.push_back(std::abs(r));
        if(ret_win.size()>size_t(ret_cap)) ret_win.pop_front();
        if(!calib_frozen)
        {
            ticks_since_scale+=1;
            if(int(ret_win.size())>=ret_cap || ticks_since_scale>=50)
            {
                std::vector<double> v(ret_win.begin(),ret_win.end());
                if(!v.empty())
                {
                    int k=int(v.size());
                    int i10=std::max(0,k*10/100);
                    int i90=std::min(k-1,k*90/100);
                    std::nth_element(v.begin(),v.begin()+i10,v.end());
                    double p10=v[i10];
                    std::nth_element(v.begin(),v.begin()+i90,v.end());
                    double p90=v[i90];
                    double iqr=p90-p10;
                    if(iqr<1e-8) iqr=1e-8;
                    calib_scale=0.7413*(p90+p10)+0.2587*iqr;
                    if(calib_scale<1e-8) calib_scale=1e-8;
                }
                ticks_since_scale=0;
                if(int(ret_win.size())>=ret_cap) calib_frozen=true;
            }
        }
    }
}

double InferenceEngine::r1_at(int idx) const
{
    if(idx<=0) return 0.0;
    double a=prices[idx-1];
    double b=prices[idx];
    if(a<=0.0||b<=0.0) return 0.0;
    return std::log(b/a);
}

double InferenceEngine::rN_at(int idx,int n) const
{
    if(idx<n) return 0.0;
    double a=prices[idx-n];
    double b=prices[idx];
    if(a<=0.0||b<=0.0) return 0.0;
    return std::log(b/a);
}

double InferenceEngine::std_window(int idx,int n) const
{
    if(idx<n-1) return 0.0;
    double s=0.0;
    for(int k=idx-n+1;k<=idx;++k) s+=r1_at(k);
    double m=s/n;
    double v=0.0;
    for(int k=idx-n+1;k<=idx;++k)
    {
        double e=r1_at(k)-m;
        v+=e*e;
    }
    v/=n;
    return std::sqrt(v);
}

double InferenceEngine::compute_score()
{
    int N=int(prices.size());
    if(N<2) return prev_score;
    double r_pred;
    if(have_model&&N>=L+1)
    {
        torch::Tensor past=torch::empty({1,L,6},torch::kFloat32);
        float* x=past.data_ptr<float>();
        for(int i=0;i<L;++i)
        {
            int idx=N-L+i;
            double r1=r1_at(idx);
            double r5=rN_at(idx,5);
            double r20=rN_at(idx,20);
            double v5=std_window(idx,5);
            double v20=std_window(idx,20);
            qint64 ts=times[idx];
            QDateTime dt=QDateTime::fromMSecsSinceEpoch(ts,Qt::UTC);
            int sod=dt.time().hour()*3600+dt.time().minute()*60+dt.time().second();
            double min_of_day=double(sod)/60.0;
            double sin_t=std::sin(6.283185307179586*(std::fmod(min_of_day,1440.0)/1440.0));
            x[i*6+0]=float(r1);
            x[i*6+1]=float(r5);
            x[i*6+2]=float(r20);
            x[i*6+3]=float(v5);
            x[i*6+4]=float(v20);
            x[i*6+5]=float(sin_t);
        }
        torch::Tensor fut=torch::empty({1,1,2},torch::kFloat32);
        qint64 next_ts=times.back()+60000;
        QDateTime ndt=QDateTime::fromMSecsSinceEpoch(next_ts,Qt::UTC);
        int nsod=ndt.time().hour()*3600+ndt.time().minute()*60+ndt.time().second();
        double nmin=double(nsod)/60.0;
        double fsin=std::sin(6.283185307179586*(std::fmod(nmin,1440.0)/1440.0));
        double fcos=std::cos(6.283185307179586*(std::fmod(nmin,1440.0)/1440.0));
        fut[0][0][0]=float(fsin);
        fut[0][0][1]=float(fcos);
        torch::NoGradGuard ng;
        std::vector<torch::jit::IValue> inp;
        inp.push_back(past);
        inp.push_back(fut);
        torch::Tensor y=model.forward(inp).toTensor();
        r_pred=y[0][0].item<double>();
    }
    else
    {
        r_pred=std::log(prices.back()/prices[N-2]);
    }
    ema_pred_mean+=ema_pred_alpha*(r_pred-ema_pred_mean);
    double centered=r_pred-ema_pred_mean;
    double scale=calib_scale;
    if(scale<1e-8) scale=1e-8;
    double z=centered/scale;
    if(z>8.0) z=8.0;
    if(z<-8.0) z=-8.0;
    double s_sig=50.0+42.0*std::tanh(0.45*z);
    double alpha=0.25;
    double s_ema=prev_score+alpha*(s_sig-prev_score);
    double max_step=3.0;
    double delta=s_ema-prev_score;
    if(delta>max_step) delta=max_step;
    if(delta<-max_step) delta=-max_step;
    double s=prev_score+delta;
    if(std::abs(s_sig-50.0)<3.0) s+=0.03*(50.0-s);
    if(s<2.0) s=2.0;
    if(s>98.0) s=98.0;
    prev_score=s;
    return s;
}

void InferenceEngine::on_tick(qint64 ts_ms,double price)
{
    push_price(ts_ms,price);
    double s=compute_score();
    emit score(s);
}
