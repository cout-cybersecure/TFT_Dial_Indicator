#include "main_window.h"
#include "dial_widget.h"
#include "trainer_invoker.h"
#include "data_fetcher.h"
#include "inference_engine.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    root=new QWidget(this);
    setCentralWidget(root);
    search_edit=new QLineEdit(this);
    search_edit->setPlaceholderText("Search or type symbol");
    ticker_combo=new QComboBox(this);
    ticker_combo->addItem("");
    ticker_combo->addItems(QStringList()<<"AAPL"<<"MSFT"<<"GOOG"<<"AMZN"<<"TSLA"<<"NVDA"<<"META"<<"IWM"<<"SPY"<<"QQQ"
                                       <<"COINBASE:BTC-USD"<<"KRAKEN:BTCUSD"<<"BINANCEUS:BTCUSDT");
    start_btn=new QPushButton("Start",this);
    start_btn->setEnabled(false);
    status_label=new QLabel("Model ready",this);
    QHBoxLayout* top=new QHBoxLayout();
    top->addWidget(search_edit,1);
    top->addWidget(ticker_combo,0);
    top->addWidget(start_btn,0);
    QVBoxLayout* v=new QVBoxLayout();
    v->addLayout(top);
    v->addWidget(status_label);
    dial=new DialWidget(this);
    v->addWidget(dial,1);
    root->setLayout(v);
    trainer=new TrainerInvoker(this);
    connect(trainer,&TrainerInvoker::status,this,&MainWindow::on_status);
    connect(trainer,&TrainerInvoker::finished_ok,this,&MainWindow::on_trainer_done);
    connect(trainer,&TrainerInvoker::failed,this,&MainWindow::on_trainer_failed);
    connect(ticker_combo,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_ticker_selected);
    connect(search_edit,&QLineEdit::textChanged,this,&MainWindow::on_search_edited);
    connect(search_edit,&QLineEdit::returnPressed,this,&MainWindow::on_start);
    connect(start_btn,&QPushButton::clicked,this,&MainWindow::on_start);
    resize(1100,720);
}

MainWindow::~MainWindow()
{
    delete fetcher;
    delete engine;
}

void MainWindow::on_status(const QString& s)
{
    status_label->setText(s);
}

void MainWindow::on_trainer_done(const QString& path)
{
    training_running=false;
    start_btn->setText("Start");
    status_label->setText("Loading model");
    if(engine) delete engine;
    engine=new InferenceEngine(this);
    if(!engine->load(path))
    {
        status_label->setText("Model load failed");
        update_start_enabled();
        return;
    }
    if(fetcher) delete fetcher;
    fetcher=new DataFetcher(this);
    connect(fetcher,&DataFetcher::tick,engine,&InferenceEngine::on_tick);
    connect(engine,&InferenceEngine::score,this,&MainWindow::on_score);
    fetcher->start(current_symbol);
    status_label->setText("Streaming");
    update_start_enabled();
}

void MainWindow::on_trainer_failed(const QString& reason)
{
    training_running=false;
    start_btn->setText("Start");
    status_label->setText(reason);
    update_start_enabled();
}

void MainWindow::on_ticker_selected(int)
{
    update_start_enabled();
}

void MainWindow::on_search_edited(const QString& text)
{
    QString q=text.trimmed().toUpper();
    if(q.isEmpty()) return;
    int idx=ticker_combo->findText(q);
    if(idx>=0) ticker_combo->setCurrentIndex(idx);
}

void MainWindow::on_start()
{
    if(training_running) return;
    QString sym=ticker_combo->currentText().trimmed();
    if(sym.isEmpty()) sym=search_edit->text().trimmed();
    if(sym.isEmpty()) return;
    current_symbol=sym;
    training_running=true;
    start_btn->setText("Running");
    status_label->setText("Training or loading");
    trainer->start(sym);
    update_start_enabled();
}

void MainWindow::on_score(double s)
{
    double x=std::clamp(s,0.0,100.0);
    dial->set_value(x);
    if(s<33.3) status_label->setText("Strong sell");
    else if(s<50.0) status_label->setText("Sell");
    else if(s<66.6) status_label->setText("Buy");
    else status_label->setText("Strong buy");
}

void MainWindow::update_start_enabled()
{
    bool has_combo=!ticker_combo->currentText().trimmed().isEmpty();
    bool has_search=!search_edit->text().trimmed().isEmpty();
    bool ok=!training_running&&(has_combo||has_search);
    start_btn->setEnabled(ok);
}
