#pragma once

#include "benchmark.h"
#include "simulatorwindow.h"
#include "solverwindow.h"
#include "ui_settingswindow.h"
#include <QMainWindow>

class QCloseEvent;

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *e);

private slots:

    void on_SimulatorButton_clicked();

    void on_SolverButton_clicked();

    void on_beginnerRadioButton_clicked();

    void on_intermediateRadioButton_clicked();

    void on_expertRadioButton_clicked();

    void on_customRadioButton_clicked();

    void simWindowDestroyed();

    void solverWindowDestroyed();

    void on_bothButton_clicked();

    void on_benchmarkButton_clicked();

    void benchmarkDone();

private:
    std::unique_ptr<Benchmark> benchmark;
    std::unique_ptr<Ui::SettingsWindow> ui;
    std::unique_ptr<SimulatorWindow> simulator;
    std::unique_ptr<SolverWindow> solver;
};
