#include "headers/settingswindow.h"

#include "headers/benchmark.h"
#include "headers/dugtype.h"
#include "headers/problemparameters.h"
#include "headers/simulatorwindow.h"
#include "headers/solverwindow.h"
#include "ui_settingswindow.h"
#include <QCloseEvent>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow(parent), ui(std::make_unique<Ui::SettingsWindow>())
{
    ui->setupUi(this);
    ui->customRadioButton->setChecked(true);
}

void SettingsWindow::on_SimulatorButton_clicked()
{
    ProblemParameters params = {ui->widthSpinner->value(),
                                ui->heightSpinner->value(),
                                ui->bombsSpinner->value(),
                                ui->rupoorsSpinner->value()};
    if (params.height * params.width > 0) {
        if (params.height * params.width > params.bombs + params.rupoors) {
            if (simulator != nullptr) {
                simulator->close();
            }
            simulator = std::make_unique<SimulatorWindow>(params);

            connect(simulator.get(),
                    SIGNAL(closing()),
                    this,
                    SLOT(simWindowDestroyed()));

            simulator->show();
        }
    }
}

void SettingsWindow::on_SolverButton_clicked()
{
    ProblemParameters params = {ui->widthSpinner->value(),
                                ui->heightSpinner->value(),
                                ui->bombsSpinner->value(),
                                ui->rupoorsSpinner->value()};
    if (params.height * params.width > 0) {
        if (params.height * params.width > params.bombs + params.rupoors) {
            if (solver != nullptr) {
                solver->close();
            }
            solver = std::make_unique<SolverWindow>(params);

            connect(solver.get(),
                    SIGNAL(closing()),
                    this,
                    SLOT(solverWindowDestroyed()));

            solver->show();
        }
    }
}

void SettingsWindow::on_beginnerRadioButton_clicked()
{
    ui->widthSpinner->setValue(5);
    ui->widthSpinner->setEnabled(false);
    ui->heightSpinner->setValue(4);
    ui->heightSpinner->setEnabled(false);
    ui->bombsSpinner->setValue(4);
    ui->bombsSpinner->setEnabled(false);
    ui->rupoorsSpinner->setValue(0);
    ui->rupoorsSpinner->setEnabled(false);
}

void SettingsWindow::on_intermediateRadioButton_clicked()
{
    ui->widthSpinner->setValue(6);
    ui->widthSpinner->setEnabled(false);
    ui->heightSpinner->setValue(5);
    ui->heightSpinner->setEnabled(false);
    ui->bombsSpinner->setValue(4);
    ui->bombsSpinner->setEnabled(false);
    ui->rupoorsSpinner->setValue(4);
    ui->rupoorsSpinner->setEnabled(false);
}

void SettingsWindow::on_expertRadioButton_clicked()
{
    ui->widthSpinner->setValue(8);
    ui->widthSpinner->setEnabled(false);
    ui->heightSpinner->setValue(5);
    ui->heightSpinner->setEnabled(false);
    ui->bombsSpinner->setValue(8);
    ui->bombsSpinner->setEnabled(false);
    ui->rupoorsSpinner->setValue(8);
    ui->rupoorsSpinner->setEnabled(false);
}

void SettingsWindow::on_customRadioButton_clicked()
{
    ui->widthSpinner->setEnabled(true);
    ui->heightSpinner->setEnabled(true);
    ui->bombsSpinner->setEnabled(true);
    ui->rupoorsSpinner->setEnabled(true);
}

void SettingsWindow::simWindowDestroyed()
{
    simulator = nullptr;
}

void SettingsWindow::solverWindowDestroyed()
{
    solver = nullptr;
}
void SettingsWindow::closeEvent(QCloseEvent *e)
{
    if (simulator != nullptr) {
        simulator->close();
    }
    if (solver != nullptr) {
        solver->close();
    }
    e->accept();
}

void SettingsWindow::on_bothButton_clicked()
{
    ProblemParameters params = {ui->widthSpinner->value(),
                                ui->heightSpinner->value(),
                                ui->bombsSpinner->value(),
                                ui->rupoorsSpinner->value()};
    if (params.height * params.width > 0) {
        if (params.height * params.width > params.bombs + params.rupoors) {
            solver = std::make_unique<SolverWindow>(params);
            simulator = std::make_unique<SimulatorWindow>(params);

            connect(simulator.get(),
                    SIGNAL(closing()),
                    this,
                    SLOT(simWindowDestroyed()));

            connect(solver.get(),
                    SIGNAL(closing()),
                    this,
                    SLOT(solverWindowDestroyed()));

            connect(simulator.get(),
                    SIGNAL(openedCell(int, int, DugType::DugType)),
                    solver.get(),
                    SLOT(cellOpened(int, int, DugType::DugType)));

            connect(solver.get(),
                    SIGNAL(destroyed(QObject *)),
                    simulator.get(),
                    SLOT(close()));

            connect(simulator.get(),
                    SIGNAL(destroyed(QObject *)),
                    solver.get(),
                    SLOT(close()));

            simulator->show();
            solver->show();
        }
    }
}

void SettingsWindow::on_benchmarkButton_clicked()
{
    auto params = ProblemParameters{ui->widthSpinner->value(),
                                    ui->heightSpinner->value(),
                                    ui->bombsSpinner->value(),
                                    ui->rupoorsSpinner->value()};
    if (params.height * params.width > 0 &&
        params.height * params.width > params.bombs + params.rupoors) {
        ui->SimulatorButton->setEnabled(false);
        ui->SolverButton->setEnabled(false);
        ui->bothButton->setEnabled(false);
        ui->benchmarkButton->setEnabled(false);

        benchmark = std::make_unique<Benchmark>(params);
        connect(benchmark.get(), SIGNAL(done()), this, SLOT(benchmarkDone()));
        benchmark->start();
    }
}

void SettingsWindow::benchmarkDone()
{
    ui->SimulatorButton->setEnabled(true);
    ui->SolverButton->setEnabled(true);
    ui->bothButton->setEnabled(true);
    ui->benchmarkButton->setEnabled(true);
    benchmark = nullptr;
}
