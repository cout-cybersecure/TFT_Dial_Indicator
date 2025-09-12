#pragma once
#include <QObject>
#include <QTimer>

class DataFetcher : public QObject
{
    Q_OBJECT
public:
    explicit DataFetcher(QObject* parent = nullptr);
    void trigger();

signals:
    void priceUpdated(double price);

private slots:
    void generatePrice();

private:
    QTimer timer;
    double currentPrice;
};
