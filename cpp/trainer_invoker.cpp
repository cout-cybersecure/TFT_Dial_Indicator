#include "trainer_invoker.h"
#include <QDir>
#include <QCoreApplication>

TrainerInvoker::TrainerInvoker(QObject* parent) : QObject(parent)
{
    connect(&proc, &QProcess::readyReadStandardOutput, this, &TrainerInvoker::on_ready);
    connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &TrainerInvoker::on_finished);
}

void TrainerInvoker::start(const QString& symbol)
{
    QString appdir = QCoreApplication::applicationDirPath();
    QString python = "python3";
    QString script = QDir(appdir).filePath("../python/train_tft.py");
    QString model_dir = QDir(appdir).filePath("../models");
    QDir().mkpath(model_dir);
    out_path = QDir(model_dir).filePath(symbol + ".pt");
    QStringList args;
    args << script << "--symbol" << symbol << "--out" << out_path;
    proc.setProgram(python);
    proc.setArguments(args);
    proc.setProcessChannelMode(QProcess::MergedChannels);
    emit status("Training");
    proc.start();
}

void TrainerInvoker::on_ready()
{
    QByteArray b = proc.readAll();
    emit status(QString::fromUtf8(b));
}

void TrainerInvoker::on_finished(int code, QProcess::ExitStatus st)
{
    if (st == QProcess::CrashExit)
    {
        emit failed("Trainer crashed");
        return;
    }
    if (code != 0)
    {
        emit failed("Trainer failed");
        return;
    }
    emit finished_ok(out_path);
}
