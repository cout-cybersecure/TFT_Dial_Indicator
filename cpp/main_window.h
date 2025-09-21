#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class DialWidget;
class TrainerInvoker;
class DataFetcher;
class InferenceEngine;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent=nullptr);
    ~MainWindow();
private slots:
    void on_status(const QString& s);
    void on_trainer_done(const QString& model_path);
    void on_trainer_failed(const QString& reason);
    void on_ticker_selected(int idx);
    void on_search_edited(const QString& text);
    void on_start();
    void on_score(double s);
private:
    void update_start_enabled();
    QWidget* root=nullptr;
    QLineEdit* search_edit=nullptr;
    QComboBox* ticker_combo=nullptr;
    QPushButton* start_btn=nullptr;
    QLabel* status_label=nullptr;
    DialWidget* dial=nullptr;
    TrainerInvoker* trainer=nullptr;
    DataFetcher* fetcher=nullptr;
    InferenceEngine* engine=nullptr;
    bool training_running=false;
    QString current_symbol;
};
