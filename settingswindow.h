#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class SettingsWindow;
}
class SimulatorWindow;
class SolverWindow;
class QCloseEvent;

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();
    void closeEvent(QCloseEvent * e);

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

private:
    Ui::SettingsWindow *ui;
    SimulatorWindow * simulator;
    SolverWindow * solver;


};

#endif // MAINWINDOW_H
