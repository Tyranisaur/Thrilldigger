#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "simulatorwindow.h"
#include "board.h"


SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    ui->customRadioButton->setChecked(true);
    simulator = nullptr;
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
    simulator->close();
    delete simulator;
}



void SettingsWindow::on_SimulatorButton_clicked()
{
    ProblemParameters params = {
        ui->widthSpinner->value(),
        ui->heightSpinner->value(),
        ui->bombsSpinner->value(),
        ui->rupoorsSpinner->value()
    };
    if(simulator != nullptr)
    {
        simulator->close();
        delete simulator;
    }
    simulator = new SimulatorWindow(&params);
    simulator->show();
}

void SettingsWindow::on_SolverButton_clicked()
{

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
