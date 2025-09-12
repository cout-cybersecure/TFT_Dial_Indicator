#include "TrainerInvoker.h"
#include <QCoreApplication>
#include <QDir>
#include <QProcessEnvironment>

TrainerInvoker::TrainerInvoker(QObject* parent) : QObject(parent) {
    proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc, &QProcess::readyReadStandardOutput, this, &TrainerInvoker::onReady);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TrainerInvoker::onFinished);
}

void TrainerInvoker::start(const QString& symbol) {
    if (proc->state() != QProcess::NotRunning) return;
    symbol_ = symbol.trimmed();
    lastStatus_.clear();
    emit status("Running...");

    const QString exe = "python3";
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString script = QDir(appDir).filePath("../python/train_tft.py");
    const QString outDir = QDir(appDir).filePath("../models");

    QStringList args{script, symbol_, outDir};
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("PYTHONUNBUFFERED", "1");
    proc->setProcessEnvironment(env);
    proc->start(exe, args);
}

void TrainerInvoker::onReady() {
    while (proc->canReadLine()) {
        const QByteArray raw = proc->readLine().trimmed();
        if (raw.startsWith("STATUS:")) {
            lastStatus_ = QString::fromUtf8(raw.mid(7)).trimmed();
            emit status(lastStatus_);
        }
    }
}

void TrainerInvoker::onFinished(int code, QProcess::ExitStatus st) {
    if (st == QProcess::NormalExit && code == 0) {
        emit status("Model ready");
        emit finishedOk("models/model.onnx");
        return;
    }
    QString reason = lastStatus_;
    if (reason.isEmpty()) {
        QByteArray out = proc->readAllStandardOutput();
        if (out.isEmpty()) out = proc->readAllStandardError();
        reason = QString::fromUtf8(out).trimmed();
        if (reason.isEmpty()) reason = QString("Trainer exited with code %1").arg(code);
    }
    emit failed(QString("Failed to get ticker '%1': %2").arg(symbol_, reason));
}
