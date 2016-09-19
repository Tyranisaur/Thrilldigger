#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

private slots:

    void on_SimulatorButton_clicked();

    void on_SolverButton_clicked();

private:
    Ui::SettingsWindow *ui;


};

#endif // MAINWINDOW_H
