#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}



void SettingsWindow::on_SimulatorButton_clicked()
{

}

void SettingsWindow::on_SolverButton_clicked()
{

}
