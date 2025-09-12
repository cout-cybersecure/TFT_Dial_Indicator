#include "MainWindow.h"
#include "DialWidget.h"
#include "TrainerInvoker.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    root = new QWidget(this);
    setCentralWidget(root);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search tickers...");

    tickerCombo = new QComboBox(this);
    tickerCombo->addItem("");
    tickerCombo->addItems(QStringList()
                          << "AAPL" << "MSFT" << "GOOG" << "AMZN" << "TSLA"
                          << "NVDA" << "META" << "IWM"  << "SPY"  << "QQQ");

    startBtn = new QPushButton("Start", this);
    startBtn->setEnabled(false);

    statusLabel = new QLabel("Model ready", this);

    QHBoxLayout* top = new QHBoxLayout();
    top->addWidget(searchEdit, 1);
    top->addWidget(tickerCombo, 0);
    top->addWidget(startBtn, 0);

    QVBoxLayout* v = new QVBoxLayout();
    v->addLayout(top);
    v->addWidget(statusLabel);
    dial = new DialWidget(this);
    v->addWidget(dial, 1);
    root->setLayout(v);

    trainer = new TrainerInvoker(this);
    connect(trainer, &TrainerInvoker::status, this, &MainWindow::onStatus);
    connect(trainer, &TrainerInvoker::finishedOk, this, &MainWindow::onTrainerDone);
    connect(trainer, &TrainerInvoker::failed, this, &MainWindow::onTrainingFailed);

    connect(tickerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onTickerSelected);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchEdited);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStart);

    resize(1100, 720);
}

MainWindow::~MainWindow() {}

void MainWindow::onStatus(const QString& s) {
    statusLabel->setText(s);
}

void MainWindow::onTrainerDone(const QString&) {
    trainingRunning = false;
    startBtn->setText("Start");
    statusLabel->setText("Model ready");
    updateStartEnabled();
}

void MainWindow::onTrainingFailed(const QString& reason) {
    trainingRunning = false;
    startBtn->setText("Start");
    statusLabel->setText(reason);
    updateStartEnabled();
}

void MainWindow::onTickerSelected(int) {
    updateStartEnabled();
}

void MainWindow::onSearchEdited(const QString& text) {
    const QString q = text.trimmed().toUpper();
    if (q.isEmpty()) return;
    int idx = tickerCombo->findText(q);
    if (idx >= 0) tickerCombo->setCurrentIndex(idx);
}

void MainWindow::onStart() {
    if (trainingRunning) return;
    const QString sym = tickerCombo->currentText().trimmed();
    if (sym.isEmpty()) return;
    trainingRunning = true;
    startBtn->setText("Running...");
    statusLabel->setText("Running...");
    trainer->start(sym);
    updateStartEnabled();
}

void MainWindow::updateStartEnabled() {
    const bool ok = !trainingRunning && !tickerCombo->currentText().trimmed().isEmpty();
    startBtn->setEnabled(ok);
}
