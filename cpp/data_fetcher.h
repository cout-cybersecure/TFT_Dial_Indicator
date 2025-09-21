#pragma once
#include <QObject>
#include <QTimer>
#include <QWebSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVector>

class DataFetcher:public QObject
{
    Q_OBJECT
public:
    explicit DataFetcher(QObject* parent=nullptr);
    void start(const QString& symbol);
signals:
    void tick(qint64 ts_ms,double price);
private slots:
    void do_connect();
    void on_ws_connected();
    void on_ws_text(const QString& m);
    void on_ws_closed();
    void on_ws_error(QAbstractSocket::SocketError);
    void on_backfill_http_finished();
    void on_reconnect();
private:
    void begin_backfill();
    void open_ws();
    void emit_sorted_backfill();
    QString sym;
    bool use_binance=false;
    bool use_binanceus=false;
    bool use_coinbase=false;
    bool use_kraken=false;
    QString ws_url;
    QWebSocket ws;
    QNetworkAccessManager nam;
    QNetworkReply* pending=nullptr;
    QVector<QPair<qint64,double>> backfill;
    int target_backfill=800;
    bool backfilling=false;
    QString coinbase_prod;
    qint64 coinbase_before_tid=0;
    QString kraken_pair;
    QString kraken_since;
    QTimer reconnect_timer;
};
