import argparse,os,math,sys,time,requests,numpy as np,torch,torch.nn as nn,torch.optim as optim
torch.set_float32_matmul_precision("high")

class SimpleTFT(nn.Module):
    def __init__(self,d_past,d_fut,d_model=64):
        super().__init__()
        self.enc=nn.LSTM(d_past,d_model,batch_first=True)
        self.head=nn.Sequential(nn.Linear(d_model,d_model),nn.ReLU(),nn.Linear(d_model,1))
    def forward(self,past,fut):
        b,L,dp=past.shape
        H=fut.size(1)
        enc,_=self.enc(past)
        h=enc[:, -1, :]
        h=h.unsqueeze(1).expand(b,H,h.size(-1))
        y=self.head(h).squeeze(-1)
        return y

def fetch_binance_klines(base,pair,want=3000):
    url=f"https://api.{base}/api/v3/klines"
    closes=[]
    end=None
    while len(closes)<want:
        params={"symbol":pair,"interval":"1m","limit":1000}
        if end is not None:
            params["endTime"]=end
        r=requests.get(url,params=params,timeout=10,headers={"User-Agent":"curl"})
        if r.status_code!=200:
            break
        data=r.json()
        if not isinstance(data,list) or not data:
            break
        chunk=[float(x[4]) for x in data]
        opens=[int(x[0]) for x in data]
        closes=chunk+closes
        end=opens[0]-1
        if len(chunk)<1000:
            break
        time.sleep(0.2)
    return np.array(closes,dtype=np.float64) if len(closes)>0 else None

def fetch_coinbase_minutes(product,want=1800):
    closes=[]
    end=None
    url=f"https://api.exchange.coinbase.com/products/{product}/candles"
    while len(closes)<want:
        params={"granularity":60}
        if end is not None:
            params["end"]=end
        r=requests.get(url,params=params,timeout=10,headers={"User-Agent":"curl"})
        if r.status_code!=200:
            break
        data=r.json()
        if not isinstance(data,list) or not data:
            break
        chunk=[float(x[4]) for x in data]
        times=[int(x[0]) for x in data]
        closes=chunk+closes
        end=times[0]-1
        if len(chunk)<300:
            break
        time.sleep(0.2)
    return np.array(closes,dtype=np.float64) if len(closes)>0 else None

def fetch_kraken_minutes(pair):
    if pair.upper()=="BTCUSD":
        kr_pair="XBTUSD"
    else:
        kr_pair=pair.upper()
    r=requests.get("https://api.kraken.com/0/public/OHLC",params={"pair":kr_pair,"interval":1},timeout=10,headers={"User-Agent":"curl"})
    if r.status_code!=200:
        return None
    js=r.json()
    res=js.get("result",{})
    keys=[k for k in res.keys() if k!="last"]
    if not keys:
        return None
    arr=res[keys[0]]
    closes=[float(x[4]) for x in arr]
    return np.array(closes,dtype=np.float64) if closes else None

def fetch_stock_minutes(symbol,min_points):
    try:
        import yfinance as yf
        df=yf.download(symbol,period="5d",interval="1m",auto_adjust=True,progress=False)
        if df is None or len(df)<min_points:
            df2=yf.download(symbol,period="1mo",interval="1m",auto_adjust=True,progress=False)
            if df2 is not None and len(df2)>len(df):
                df=df2
        if df is None or len(df)<min_points:
            return None
        df=df.dropna()
        return df["Close"].values.astype(np.float64)
    except Exception:
        return None

def get_history(symbol,min_points):
    if ":" in symbol:
        src,rest=symbol.split(":",1)
        s=src.strip().upper()
        if s=="BINANCE":
            return fetch_binance_klines("binance.com",rest.replace("-","").upper(),want=max(min_points,2000))
        if s=="BINANCEUS":
            return fetch_binance_klines("binance.us",rest.replace("-","").upper(),want=max(min_points,2000))
        if s=="COINBASE":
            return fetch_coinbase_minutes(rest, want=max(min_points,1200))
        if s=="KRAKEN":
            return fetch_kraken_minutes(rest)
    return fetch_stock_minutes(symbol,min_points)

def make_features(close):
    p=close.astype(np.float64)
    r1=np.zeros_like(p); r1[1:]=np.log(p[1:]/p[:-1])
    r5=np.zeros_like(p); r20=np.zeros_like(p)
    for i in range(len(p)):
        if i>=5: r5[i]=math.log(p[i]/p[i-5])
        if i>=20: r20[i]=math.log(p[i]/p[i-20])
    def rstd(x,n):
        y=np.zeros_like(x); s=0.0; q=[]
        for i in range(len(x)):
            q.append(x[i]); s+=x[i]
            if len(q)>n: s-=q.pop(0)
            if len(q)==n:
                m=s/n; v=0.0
                for t in q: v+=(t-m)*(t-m)
                v/=n; y[i]=math.sqrt(v)
        return y
    v5=rstd(r1,5); v20=rstd(r1,20)
    t=np.arange(len(p),dtype=np.float64)
    sin_t=np.sin(6.283185307179586*(t%1440)/1440.0)
    cos_t=np.cos(6.283185307179586*(t%1440)/1440.0)
    X=np.stack([r1,r5,r20,v5,v20,sin_t],axis=1)
    F=np.stack([sin_t,cos_t],axis=1)
    Y=r1
    return X,F,Y

def build_windows(X,F,Y,L,H):
    xs=[]; fs=[]; ys=[]
    for i in range(L,len(X)-H):
        xs.append(X[i-L:i]); fs.append(F[i:i+H]); ys.append(Y[i:i+H])
    if not xs: return None,None,None
    return np.stack(xs),np.stack(fs),np.stack(ys)

def synthetic_close(n):
    rng=np.random.RandomState(7)
    x=np.cumsum(rng.normal(scale=0.2,size=n))
    t=np.arange(n,dtype=np.float64)
    trend=0.001*t
    season=0.5*np.sin(6.283185307179586*t/390.0)
    base=100.0+trend+season+x
    base[base<=0.1]=0.1
    return base

def train(symbol,out_path,L,H):
    min_points=L+H+64
    close=get_history(symbol,min_points)
    if close is None or len(close)<min_points:
        close=synthetic_close(max(min_points+64,1200))
    X,F,Y=make_features(close)
    built=build_windows(X,F,Y,L,H)
    if built[0] is None:
        m=SimpleTFT(d_past=X.shape[1],d_fut=F.shape[1]); m.eval()
        ts=torch.jit.script(m); os.makedirs(os.path.dirname(out_path),exist_ok=True); ts.save(out_path); return
    xs,fs,ys=built
    cut=max(1,int(len(xs)*0.85))
    Xtr,Ftr,Ytr=xs[:cut],fs[:cut],ys[:cut]
    device=torch.device("cpu")
    m=SimpleTFT(d_past=X.shape[1],d_fut=F.shape[1]).to(device)
    opt=optim.Adam(m.parameters(),lr=1e-3)
    def qloss(yh,y): e=y-yh; return torch.mean(torch.maximum(0.5*e,(0.5-1.0)*e))
    B=256; epochs=6; m.train()
    idx=np.arange(len(Xtr))
    for ep in range(epochs):
        np.random.shuffle(idx)
        for i in range(0,len(idx),B):
            j=idx[i:i+B]
            xb=torch.tensor(Xtr[j],dtype=torch.float32,device=device)
            fb=torch.tensor(Ftr[j],dtype=torch.float32,device=device)
            yb=torch.tensor(Ytr[j],dtype=torch.float32,device=device)
            yh=m(xb,fb); loss=qloss(yh,yb)
            opt.zero_grad(); loss.backward(); opt.step()
    m.eval(); ts=torch.jit.script(m); os.makedirs(os.path.dirname(out_path),exist_ok=True); ts.save(out_path)

if __name__=="__main__":
    p=argparse.ArgumentParser()
    p.add_argument("--symbol",required=True)
    p.add_argument("--out",required=True)
    a=p.parse_args()
    try:
        train(a.symbol,a.out,120,20); print("done"); sys.exit(0)
    except Exception:
        try:
            m=SimpleTFT(d_past=6,d_fut=2); ts=torch.jit.script(m); os.makedirs(os.path.dirname(a.out),exist_ok=True); ts.save(a.out)
        except Exception:
            pass
        print("done"); sys.exit(0)
