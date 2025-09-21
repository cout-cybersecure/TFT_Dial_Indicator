#include "data_fetcher.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <algorithm>

DataFetcher::DataFetcher(QObject* parent):QObject(parent)
{
    connect(&ws,&QWebSocket::connected,this,&DataFetcher::on_ws_connected);
    connect(&ws,&QWebSocket::textMessageReceived,this,&DataFetcher::on_ws_text);
    connect(&ws,&QWebSocket::disconnected,this,&DataFetcher::on_ws_closed);
#if QT_VERSION >= QT_VERSION_CHECK(6,5,0)
    connect(&ws,&QWebSocket::errorOccurred,this,&DataFetcher::on_ws_error);
#else
    connect(&ws,static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),this,&DataFetcher::on_ws_error);
#endif
    connect(&reconnect_timer,&QTimer::timeout,this,&DataFetcher::do_connect);
}

void DataFetcher::start(const QString& symbol)
{
    sym=symbol.trimmed();
    use_binance=false;
    use_binanceus=false;
    use_coinbase=false;
    use_kraken=false;
    ws_url.clear();
    backfill.clear();
    backfilling=false;
    coinbase_before_tid=0;
    kraken_since.clear();
    reconnect_timer.stop();
    if(sym.startsWith("BINANCEUS:",Qt::CaseInsensitive))
    {
        QString pair=sym.section(':',1).toUpper();
        ws_url=QString("wss://stream.binance.us:9443/ws/%1@trade").arg(pair.toLower());
        use_binanceus=true;
    }
    else if(sym.startsWith("BINANCE:",Qt::CaseInsensitive))
    {
        QString pair=sym.section(':',1).toUpper();
        ws_url=QString("wss://stream.binance.com:9443/ws/%1@trade").arg(pair.toLower());
        use_binance=true;
    }
    else if(sym.startsWith("COINBASE:",Qt::CaseInsensitive))
    {
        coinbase_prod=sym.section(':',1);
        use_coinbase=true;
    }
    else if(sym.startsWith("KRAKEN:",Qt::CaseInsensitive))
    {
        QString p=sym.section(':',1).toUpper();
        if(p=="BTCUSD") p="XBTUSD";
        kraken_pair=p;
        use_kraken=true;
    }
    begin_backfill();
}

void DataFetcher::begin_backfill()
{
    if(use_binanceus)
    {
        QString pair=sym.section(':',1).toUpper();
        QUrl u("https://api.binance.us/api/v3/trades");
        QUrlQuery q;
        q.addQueryItem("symbol",pair);
        q.addQueryItem("limit","1000");
        u.setQuery(q);
        QNetworkRequest req(u);
        pending=nam.get(req);
        connect(pending,&QNetworkReply::finished,this,&DataFetcher::on_backfill_http_finished);
        backfilling=true;
        return;
    }
    if(use_binance)
    {
        QString pair=sym.section(':',1).toUpper();
        QUrl u("https://api.binance.com/api/v3/trades");
        QUrlQuery q;
        q.addQueryItem("symbol",pair);
        q.addQueryItem("limit","1000");
        u.setQuery(q);
        QNetworkRequest req(u);
        pending=nam.get(req);
        connect(pending,&QNetworkReply::finished,this,&DataFetcher::on_backfill_http_finished);
        backfilling=true;
        return;
    }
    if(use_coinbase)
    {
        QUrl u(QString("https://api.exchange.coinbase.com/products/%1/trades").arg(coinbase_prod));
        QUrlQuery q;
        q.addQueryItem("limit","100");
        u.setQuery(q);
        QNetworkRequest req(u);
        req.setRawHeader("User-Agent","tft_dial");
        pending=nam.get(req);
        connect(pending,&QNetworkReply::finished,this,&DataFetcher::on_backfill_http_finished);
        backfilling=true;
        return;
    }
    if(use_kraken)
    {
        QUrl u("https://api.kraken.com/0/public/Trades");
        QUrlQuery q;
        q.addQueryItem("pair",kraken_pair);
        if(!kraken_since.isEmpty()) q.addQueryItem("since",kraken_since);
        u.setQuery(q);
        QNetworkRequest req(u);
        pending=nam.get(req);
        connect(pending,&QNetworkReply::finished,this,&DataFetcher::on_backfill_http_finished);
        backfilling=true;
        return;
    }
    do_connect();
}

void DataFetcher::emit_sorted_backfill()
{
    if(backfill.isEmpty()) return;
    std::sort(backfill.begin(),backfill.end(),[](const QPair<qint64,double>& a,const QPair<qint64,double>& b){ return a.first<b.first; });
    for(auto& pr:backfill) emit tick(pr.first,pr.second);
    backfill.clear();
}

void DataFetcher::on_backfill_http_finished()
{
    if(!pending)
    {
        do_connect();
        return;
    }
    QByteArray b=pending->readAll();
    pending->deleteLater();
    pending=nullptr;
    if(b.isEmpty())
    {
        do_connect();
        return;
    }
    if(use_binance||use_binanceus)
    {
        QJsonDocument d=QJsonDocument::fromJson(b);
        if(d.isArray())
        {
            QJsonArray a=d.array();
            for(auto v:a)
            {
                QJsonObject o=v.toObject();
                double p=o.value("price").toString().toDouble();
                qint64 ts=o.value("time").toVariant().toLongLong();
                if(p>0.0&&ts>0) backfill.append({ts,p});
            }
        }
        emit_sorted_backfill();
        do_connect();
        return;
    }
    if(use_coinbase)
    {
        QJsonDocument d=QJsonDocument::fromJson(b);
        if(d.isArray())
        {
            QJsonArray a=d.array();
            qint64 min_tid=LLONG_MAX;
            for(auto v:a)
            {
                QJsonObject o=v.toObject();
                double p=o.value("price").toString().toDouble();
                QString iso=o.value("time").toString();
                QDateTime dt=QDateTime::fromString(iso,Qt::ISODateWithMs);
                if(!dt.isValid()) dt=QDateTime::fromString(iso,Qt::ISODate);
                qint64 ts=dt.toMSecsSinceEpoch();
                qint64 tid=o.value("trade_id").toVariant().toLongLong();
                if(p>0.0&&ts>0) backfill.append({ts,p});
                if(tid>0&&tid<min_tid) min_tid=tid;
            }
            bool need_more=backfill.size()<target_backfill && a.size()>0 && min_tid!=LLONG_MAX;
            if(need_more)
            {
                coinbase_before_tid=min_tid-1;
                QUrl u(QString("https://api.exchange.coinbase.com/products/%1/trades").arg(coinbase_prod));
                QUrlQuery q;
                q.addQueryItem("limit","100");
                q.addQueryItem("before",QString::number(coinbase_before_tid));
                u.setQuery(q);
                QNetworkRequest req(u);
                req.setRawHeader("User-Agent","tft_dial");
                pending=nam.get(req);
                connect(pending,&QNetworkReply::finished,this,&DataFetcher::on_backfill_http_finished);
                return;
            }
        }
        emit_sorted_backfill();
        do_connect();
        return;
    }
    if(use_kraken)
    {
        QJsonDocument d=QJsonDocument::fromJson(b);
        if(d.isObject())
        {
            QJsonObject root=d.object().value("result").toObject();
            QJsonArray arr=root.value(kraken_pair).toArray();
            for(auto v:arr)
            {
                QJsonArray t=v.toArray();
                if(t.size()>=3)
                {
                    double p=t[0].toString().toDouble();
                    double ts_sec=t[2].toDouble();
                    qint64 ts=qint64(ts_sec*1000.0);
                    if(p>0.0&&ts>0) backfill.append({ts,p});
                }
            }
            kraken_since=root.value("last").toString();
            bool need_more=backfill.size()<target_backfill && !kraken_since.isEmpty();
            if(need_more)
            {
                QUrl u("https://api.kraken.com/0/public/Trades");
                QUrlQuery q;
                q.addQueryItem("pair",kraken_pair);
                q.addQueryItem("since",kraken_since);
                u.setQuery(q);
                QNetworkRequest req(u);
                pending=nam.get(req);
                connect(pending,&QNetworkReply::finished,this,&DataFetcher::on_backfill_http_finished);
                return;
            }
        }
        emit_sorted_backfill();
        do_connect();
        return;
    }
    do_connect();
}

void DataFetcher::do_connect()
{
    reconnect_timer.stop();
    if(use_binance||use_binanceus)
    {
        ws.open(QUrl(ws_url));
        return;
    }
    if(use_coinbase)
    {
        ws.open(QUrl("wss://ws-feed.exchange.coinbase.com"));
        return;
    }
    if(use_kraken)
    {
        ws.open(QUrl("wss://ws.kraken.com"));
        return;
    }
}

void DataFetcher::on_ws_connected()
{
    if(use_binance||use_binanceus)
    {
        return;
    }
    if(use_coinbase)
    {
        QJsonArray products; products.append(coinbase_prod);
        QJsonObject chm; chm.insert("name","matches"); chm.insert("product_ids",products);
        QJsonObject cht; cht.insert("name","ticker"); chm.insert("product_ids",products);
        QJsonArray channels; channels.append(chm); channels.append(cht);
        QJsonObject o; o.insert("type","subscribe"); o.insert("channels",channels);
        ws.sendTextMessage(QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact)));
        return;
    }
    if(use_kraken)
    {
        QString p=kraken_pair;
        if(!p.contains('/')) p=p.left(3)+"/"+p.mid(3);
        QJsonObject sub; sub.insert("name","trade");
        QJsonObject o; o.insert("event","subscribe");
        QJsonArray pair; pair.append(p);
        o.insert("pair",pair);
        o.insert("subscription",sub);
        ws.sendTextMessage(QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact)));
        return;
    }
}

void DataFetcher::on_ws_text(const QString& m)
{
    QJsonParseError pe;
    QJsonDocument doc=QJsonDocument::fromJson(m.toUtf8(),&pe);
    if(use_kraken&&doc.isArray())
    {
        QJsonArray a=doc.array();
        if(a.size()>=3&&a[2].toString()=="trade")
        {
            QJsonArray trades=a[1].toArray();
            for(auto v:trades)
            {
                QJsonArray t=v.toArray();
                if(t.size()>=3)
                {
                    double p=t[0].toString().toDouble();
                    double ts_sec=t[2].toString().toDouble();
                    qint64 ts=qint64(ts_sec*1000.0);
                    if(p>0.0&&ts>0) emit tick(ts,p);
                }
            }
        }
        return;
    }
    if(!doc.isObject()) return;
    QJsonObject o=doc.object();
    if(use_binance||use_binanceus)
    {
        if(o.value("e").toString()=="trade")
        {
            double p=o.value("p").toString().toDouble();
            qint64 ts=o.value("T").toVariant().toLongLong();
            if(p>0.0&&ts>0) emit tick(ts,p);
        }
        return;
    }
    if(use_coinbase)
    {
        QString t=o.value("type").toString();
        if(t=="match"||t=="ticker")
        {
            double p=o.value("price").toString().toDouble();
            QString iso=o.value("time").toString();
            QDateTime dt=QDateTime::fromString(iso,Qt::ISODateWithMs);
            if(!dt.isValid()) dt=QDateTime::fromString(iso,Qt::ISODate);
            qint64 ts=dt.toMSecsSinceEpoch();
            if(p>0.0&&ts>0) emit tick(ts,p);
        }
        return;
    }
}

void DataFetcher::on_ws_closed()
{
    reconnect_timer.start(1000);
}

void DataFetcher::on_ws_error(QAbstractSocket::SocketError)
{
    ws.close();
}

void DataFetcher::on_reconnect()
{
    do_connect();
}
