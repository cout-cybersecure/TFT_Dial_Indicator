#include "DataFetcher.h"
#include <cstdlib>

DataFetcher::DataFetcher(QObject* parent)
    : QObject(parent), currentPrice(100.0)
{
    connect(&timer, &QTimer::timeout, this, &DataFetcher::generatePrice);
    timer.start(1000);
}

void DataFetcher::trigger()
{
    generatePrice();
}

void DataFetcher::generatePrice()
{
    currentPrice += ((std::rand() % 200) - 100) / 100.0;
    emit priceUpdated(currentPrice);
}
