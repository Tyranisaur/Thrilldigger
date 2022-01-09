#pragma once

#include "difficulty.h"

#include "ui_settingswindow.h"
#include <QMainWindow>

#include <memory>

class QCloseEvent;
class Benchmark;
class SimulatorWindow;
class SolverWindow;

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow() override;
    void closeEvent(QCloseEvent *e) override;

private:

    Ui::SettingsWindow ui{};
    std::unique_ptr<Benchmark> benchmark;
    std::unique_ptr<SimulatorWindow> simulator;
    std::unique_ptr<SolverWindow> solver;

    void setParameterSpinners(difficulty::Difficulty difficultyLevel);

    void simulatorButton_clicked();

    void solverButton_clicked();

    void beginnerRadioButton_clicked();

    void intermediateRadioButton_clicked();

    void expertRadioButton_clicked();

    void customRadioButton_clicked();

    void simWindowDestroyed();

    void solverWindowDestroyed();

    void bothButton_clicked();

    void benchmarkButton_clicked();

    void benchmarkDone();

    ProblemParameters problemParameters();
};
