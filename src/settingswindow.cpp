#include "headers/settingswindow.h"

#include "headers/benchmark.h"
#include "headers/dugtype.h"
#include "headers/problemparameters.h"
#include "headers/simulatorwindow.h"
#include "headers/solverwindow.h"
#include "ui_settingswindow.h"
#include <QCloseEvent>



SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.customRadioButton->setChecked(true);

    connect(ui.beginnerRadioButton, &QRadioButton::clicked, [this] {
        beginnerRadioButton_clicked();
    });
    connect(ui.intermediateRadioButton, &QRadioButton::clicked, [this] {
        intermediateRadioButton_clicked();
    });
    connect(ui.expertRadioButton, &QRadioButton::clicked, [this] {
        expertRadioButton_clicked();
    });
    connect(ui.customRadioButton, &QRadioButton::clicked, [this] {
        customRadioButton_clicked();
    });

    connect(ui.SimulatorButton, &QPushButton::clicked, [this] {
        simulatorButton_clicked();
    });
    connect(ui.SolverButton, &QPushButton::clicked, [this] {
        solverButton_clicked();
    });
    connect(ui.bothButton, &QPushButton::clicked, [this] {
        bothButton_clicked();
    });
    connect(ui.benchmarkButton, &QPushButton::clicked, [this] {
        benchmarkButton_clicked();
    });
}

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::simulatorButton_clicked()
{
    const auto params = problemParameters();
    if (params.height * params.width > 0) {
        if (params.height * params.width > params.bombs + params.rupoors) {
            if (simulator != nullptr) {
                simulator->close();
            }
            simulator = std::make_unique<SimulatorWindow>(params, this);

            connect(simulator.get(), &SimulatorWindow::closing, [this] {
                simWindowDestroyed();
            });

            simulator->show();
        }
    }
}

void SettingsWindow::solverButton_clicked()
{
    const auto params = problemParameters();
    if (params.height > 0 && params.width > 0) {
        if (params.height * params.width > params.bombs + params.rupoors) {
            if (solver != nullptr) {
                solver->close();
            }
            solver = std::make_unique<SolverWindow>(params, this);

            connect(solver.get(), &SolverWindow::closing, [this] {
                solverWindowDestroyed();
            });

            solver->show();
        }
    }
}

void SettingsWindow::beginnerRadioButton_clicked()
{
    setParameterSpinners(difficulty::Difficulty::BEGINNER);
}

void SettingsWindow::intermediateRadioButton_clicked()
{
    setParameterSpinners(difficulty::Difficulty::INTERMEDIATE);
}

void SettingsWindow::expertRadioButton_clicked()
{
    setParameterSpinners(difficulty::Difficulty::EXPERT);
}

void SettingsWindow::customRadioButton_clicked()
{
    ui.widthSpinner->setEnabled(true);
    ui.heightSpinner->setEnabled(true);
    ui.bombsSpinner->setEnabled(true);
    ui.rupoorsSpinner->setEnabled(true);
}

void SettingsWindow::simWindowDestroyed()
{
    simulator.release();
}

void SettingsWindow::solverWindowDestroyed()
{
    solver.release();
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

void SettingsWindow::bothButton_clicked()
{
    const auto params = problemParameters();
    if (params.height > 0 && params.width > 0) {
        if (params.height * params.width > params.bombs + params.rupoors) {
            solver = std::make_unique<SolverWindow>(params, this);
            simulator = std::make_unique<SimulatorWindow>(params, this);

            connect(simulator.get(), &SimulatorWindow::closing, this, [this] {
                simWindowDestroyed();
                if (solver) {
                    solver->close();
                }
            });

            connect(solver.get(), &SolverWindow::closing, this, [this] {
                solverWindowDestroyed();
                if (simulator) {
                    simulator->close();
                }
            });

            connect(simulator.get(),
                    &SimulatorWindow::openedCell,
                    [solver = solver.get()](const std::size_t x, const std::size_t y, const DugTypeEnum type) {
                        solver->cellOpened(x, y, type);
                    });

            simulator->show();
            solver->show();
        }
    }
}

void SettingsWindow::benchmarkButton_clicked()
{
    const auto params = problemParameters();
    if (params.height > 0 && params.width > 0 &&
        params.height * params.width > params.bombs + params.rupoors) {
        ui.SimulatorButton->setEnabled(false);
        ui.SolverButton->setEnabled(false);
        ui.bothButton->setEnabled(false);
        ui.benchmarkButton->setEnabled(false);

        benchmark = std::make_unique<Benchmark>(params);
        connect(benchmark.get(), &Benchmark::done, [this]{benchmarkDone();});
        benchmark->start();
    }
}

void SettingsWindow::benchmarkDone()
{
    ui.SimulatorButton->setEnabled(true);
    ui.SolverButton->setEnabled(true);
    ui.bothButton->setEnabled(true);
    ui.bothButton->setEnabled(true);
    ui.benchmarkButton->setEnabled(true);
    benchmark = nullptr;
}

ProblemParameters SettingsWindow::problemParameters()
{
    return ProblemParameters{std::size_t(ui.widthSpinner->value()),
                                    std::size_t(ui.heightSpinner->value()),
                                    std::size_t(ui.bombsSpinner->value()),
                                    std::size_t(ui.rupoorsSpinner->value())};
}

void SettingsWindow::setParameterSpinners(const difficulty::Difficulty difficultyLevel)
{
    const ProblemParameters params =
        difficulty::difficultyParameters(difficultyLevel);

    ui.widthSpinner->setValue(int(params.width));
    ui.widthSpinner->setEnabled(false);
    ui.heightSpinner->setValue(int(params.height));
    ui.heightSpinner->setEnabled(false);
    ui.bombsSpinner->setValue(int(params.bombs));
    ui.bombsSpinner->setEnabled(false);
    ui.rupoorsSpinner->setValue(int(params.rupoors));
    ui.rupoorsSpinner->setEnabled(false);
}
