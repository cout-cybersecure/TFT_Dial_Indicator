#pragma once
#include <QObject>
#include <QProcess>

class TrainerInvoker : public QObject
{
    Q_OBJECT
public:
    explicit TrainerInvoker(QObject* parent = nullptr);
    void start(const QString& symbol);

signals:
    void status(const QString&);
    void finished_ok(const QString& path);
    void failed(const QString& reason);

private slots:
    void on_ready();
    void on_finished(int code, QProcess::ExitStatus st);

private:
    QProcess proc;
    QString out_path;
};
