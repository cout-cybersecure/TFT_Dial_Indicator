#pragma once
#include <QObject>
#include <QProcess>
#include <QString>

class TrainerInvoker : public QObject {
    Q_OBJECT
public:
    explicit TrainerInvoker(QObject* parent = nullptr);
    Q_INVOKABLE void start(const QString& symbol);

signals:
    void status(const QString& line);
    void finishedOk(const QString& onnxPath);
    void failed(const QString& reason);

private slots:
    void onReady();
    void onFinished(int code, QProcess::ExitStatus st);

private:
    QProcess* proc = nullptr;
    QString symbol_;
    QString lastStatus_;
};
