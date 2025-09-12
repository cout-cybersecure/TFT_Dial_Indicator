#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <memory>

class DialWidget;
class TrainerInvoker;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onStatus(const QString& s);
    void onTrainerDone(const QString& onnxPath);
    void onTrainingFailed(const QString& reason);
    void onTickerSelected(int idx);
    void onSearchEdited(const QString& text);
    void onStart();

private:
    void updateStartEnabled();

    QWidget* root = nullptr;
    QLineEdit* searchEdit = nullptr;
    QComboBox* tickerCombo = nullptr;
    QPushButton* startBtn = nullptr;
    QLabel* statusLabel = nullptr;
    DialWidget* dial = nullptr;
    TrainerInvoker* trainer = nullptr;
    bool trainingRunning = false;
};
