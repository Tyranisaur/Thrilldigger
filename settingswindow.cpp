#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "simulatorwindow.h"
#include "solverwindow.h"
#include "problemparameters.h"
#include "benchmark.h"
#include "DugType.h"
#include <QCloseEvent>


SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    ui->customRadioButton->setChecked(true);
    simulator = nullptr;
    solver = nullptr;
}

SettingsWindow::~SettingsWindow()
{
    if(simulator != nullptr)
    {
        simulator->close();
    }
    if(solver != nullptr)
    {
        solver->close();
    }
    delete ui;
}




void SettingsWindow::on_SimulatorButton_clicked()
{
    ProblemParameters params = {
        ui->widthSpinner->value(),
        ui->heightSpinner->value(),
        ui->bombsSpinner->value(),
        ui->rupoorsSpinner->value()
    };
    if(params.height * params.width > 0 ){
        if(params.height * params.width > params.bombs + params.rupoors)
        {
            if(simulator != nullptr)
            {
                simulator->close();
            }
            simulator = new SimulatorWindow(&params);

            connect(simulator,
                    SIGNAL(closing()),
                    this,
                    SLOT(simWindowDestroyed()));

            simulator->show();
        }
    }
}

void SettingsWindow::on_SolverButton_clicked()
{
    ProblemParameters params = {
        ui->widthSpinner->value(),
        ui->heightSpinner->value(),
        ui->bombsSpinner->value(),
        ui->rupoorsSpinner->value()
    };
    if(params.height * params.width > 0 ){
        if(params.height * params.width > params.bombs + params.rupoors)
        {
            if(solver != nullptr)
            {
                solver->close();
            }
            solver = new SolverWindow(&params);

            connect(solver,
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
    if(simulator != nullptr)
    {
        simulator->close();
    }
    if(solver != nullptr)
    {
        solver->close();
    }
    e->accept();
}

void SettingsWindow::on_bothButton_clicked()
{
    ProblemParameters params = {
        ui->widthSpinner->value(),
        ui->heightSpinner->value(),
        ui->bombsSpinner->value(),
        ui->rupoorsSpinner->value()
    };
    if(params.height * params.width > 0 ){
        if(params.height * params.width > params.bombs + params.rupoors)
        {
            if(solver != nullptr)
            {
                solver->close();
            }
            if(simulator != nullptr)
            {
                simulator->close();
            }
            solver = new SolverWindow(&params);
            simulator = new SimulatorWindow(&params);

            connect(simulator,
                    SIGNAL(closing()),
                    this,
                    SLOT(simWindowDestroyed()));


            connect(solver,
                    SIGNAL(closing()),
                    this,
                    SLOT(solverWindowDestroyed()));

            connect(simulator,
                    SIGNAL(openedCell(int, int, DugType::DugType)),
                    solver,
                    SLOT(cellOpened(int, int, DugType::DugType)));

            connect(solver,
                    SIGNAL(destroyed(QObject*)),
                    simulator,
                    SLOT(close()));

            connect(simulator,
                    SIGNAL(destroyed(QObject*)),
                    solver,
                    SLOT(close()));

            simulator->show();
            solver->show();
        }
    }
}

void SettingsWindow::on_benchmarkButton_clicked()
{
    ProblemParameters * params = new ProblemParameters{
            ui->widthSpinner->value(),
            ui->heightSpinner->value(),
            ui->bombsSpinner->value(),
            ui->rupoorsSpinner->value()
};
    if(params->height * params->width > 0  &&
        params->height * params->width > params->bombs + params->rupoors)
        {
            ui->SimulatorButton->setEnabled(false);
            ui->SolverButton->setEnabled(false);
            ui->bothButton->setEnabled(false);
            ui->benchmarkButton->setEnabled(false);

            benchmark = new Benchmark(params);
            connect(benchmark,
                    SIGNAL(done()),
                    this,
                    SLOT(benchmarkDone()));
            benchmark->start();
        }
    else
    {
        delete params;
    }

}

void SettingsWindow::benchmarkDone()
{
    ui->SimulatorButton->setEnabled(true);
    ui->SolverButton->setEnabled(true);
    ui->bothButton->setEnabled(true);
    ui->benchmarkButton->setEnabled(true);
    delete benchmark;
}
