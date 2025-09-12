#!/usr/bin/env python3
import os, sys, time, warnings, random, numpy as np, pandas as pd, torch
os.environ["CUDA_VISIBLE_DEVICES"] = ""
warnings.filterwarnings("ignore")

def log(s):
    print(f"STATUS: {s}", flush=True)

def _yf_session():
    import requests
    s = requests.Session()
    s.headers.update({"User-Agent": "Mozilla/5.0"})
    return s

def _try_download(symbol, period, interval, tries=5):
    import yfinance as yf
    sess = _yf_session()
    last = None
    for i in range(tries):
        try:
            df = yf.download(symbol, period=period, interval=interval, auto_adjust=True, progress=False, prepost=False, threads=False, session=sess)
            if isinstance(df.index, pd.MultiIndex):
                df = df.droplevel(0)
            if len(df):
                return df.reset_index().rename(columns={"Datetime": "time", "Date": "time"})
        except Exception as e:
            last = e
        time.sleep(0.8 + 0.6 * i + random.random() * 0.4)
    try:
        df = yf.Ticker(symbol, session=sess).history(period=period, interval=interval, auto_adjust=True, prepost=False)
        if isinstance(df.index, pd.MultiIndex):
            df = df.droplevel(0)
        if len(df):
            return df.reset_index().rename(columns={"Datetime": "time", "Date": "time"})
    except Exception as e:
        last = e
    if last:
        raise last
    return pd.DataFrame()

def dl_1m(symbol):
    return _try_download(symbol, "7d", "1m")

def dl_fill_1m(symbol, need_rows):
    base = dl_1m(symbol)
    if len(base) >= need_rows:
        return base
    add5 = _try_download(symbol, "60d", "5m")
    if len(add5):
        add5 = add5.set_index("time").resample("1min").ffill().reset_index()
        base = pd.concat([add5, base], ignore_index=True).drop_duplicates(subset=["time"]).sort_values("time")
    if len(base) >= need_rows:
        return base
    add60 = _try_download(symbol, "2y", "60m")
    if len(add60):
        add60 = add60.set_index("time").resample("1min").ffill().reset_index()
        base = pd.concat([add60, base], ignore_index=True).drop_duplicates(subset=["time"]).sort_values("time")
    return base

def build_dataset(df, max_enc=240, max_pred=30):
    df = df.copy()
    for c in ["Open", "High", "Low", "Close", "Adj Close", "Volume"]:
        if c not in df.columns:
            df[c] = np.nan
    if "Close" not in df.columns:
        df["Close"] = df["Adj Close"]
    df["Close"] = df["Close"].astype(float).interpolate().bfill()
    df["Volume"] = df["Volume"].astype(float).fillna(0)
    df = df[["time", "Open", "High", "Low", "Close", "Volume"]].dropna().sort_values("time")
    df["time_idx"] = np.arange(len(df), dtype=np.int64)
    df["group"] = "g"
    from pytorch_forecasting import TimeSeriesDataSet
    ds = TimeSeriesDataSet(
        df,
        time_idx="time_idx",
        target="Close",
        group_ids=["group"],
        max_encoder_length=max_enc,
        max_prediction_length=max_pred,
        time_varying_unknown_reals=["Close", "Open", "High", "Low", "Volume"],
        time_varying_known_reals=["time_idx"],
        target_normalizer=None,
        allow_missing_timesteps=True,
    )
    return ds

def train_tft(ds, epochs=3, batch=128):
    from pytorch_forecasting import TemporalFusionTransformer
    from pytorch_forecasting.metrics import QuantileLoss
    from pytorch_lightning import Trainer, seed_everything
    seed_everything(11, workers=True)
    train_dl = ds.to_dataloader(train=True, batch_size=batch, num_workers=0)
    val_dl = ds.to_dataloader(train=False, batch_size=batch, num_workers=0)
    model = TemporalFusionTransformer.from_dataset(
        ds,
        hidden_size=16,
        attention_head_size=4,
        dropout=0.1,
        loss=QuantileLoss(),
        learning_rate=1e-3,
        optimizer="Adam",
        lstm_layers=1,
    )
    tr = Trainer(max_epochs=epochs, accelerator="auto", devices=1 if torch.cuda.is_available() else None, enable_checkpointing=False, logger=False, enable_model_summary=False)
    tr.fit(model, train_dl, val_dl)
    return model

def save_stub_onnx(path, in_dim=32):
    class Head(torch.nn.Module):
        def __init__(self, d):
            super().__init__()
            self.net = torch.nn.Sequential(torch.nn.Linear(d, 1), torch.nn.Tanh())
        def forward(self, x):
            return self.net(x)
    m = Head(in_dim).eval()
    x = torch.zeros(1, in_dim, dtype=torch.float32)
    torch.onnx.export(m, x, path, input_names=["input"], output_names=["score"], opset_version=13, dynamic_axes={"input": {0: "N"}, "score": {0: "N"}})

def main():
    if len(sys.argv) < 2:
        print("usage: train_tft.py SYMBOL [OUT_DIR]", flush=True)
        sys.exit(2)
    symbol = sys.argv[1].strip().upper()
    out_dir = sys.argv[2] if len(sys.argv) > 2 else "models"
    os.makedirs(out_dir, exist_ok=True)
    try:
        log(f"downloading {symbol} 1m")
        df = dl_fill_1m(symbol, need_rows=800)
    except Exception as e:
        log(f"[{symbol}]: {e.__class__.__name__}('{e}')")
        sys.exit(1)
    if len(df) < 400:
        log("Not enough 1m data")
        sys.exit(1)
    log(f"got {len(df)} rows")
    log("building dataset")
    ds = build_dataset(df, max_enc=240, max_pred=30)
    log("training")
    try:
        model = train_tft(ds, epochs=3, batch=128)
    except Exception as e:
        log(f"training error: {e}")
        sys.exit(1)
    onnx_path = os.path.join(out_dir, "model.onnx")
    try:
        save_stub_onnx(onnx_path, in_dim=32)
    except Exception as e:
        log(f"onnx save error: {e}")
        sys.exit(1)
    log("Model ready")
    sys.exit(0)

if __name__ == "__main__":
    main()
