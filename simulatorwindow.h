#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QMainWindow>

namespace Ui {
class SimulatorWindow;
}
class QPushButton;

struct ProblemParameters;
class Board;

class SimulatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulatorWindow(ProblemParameters * params, QWidget *parent = 0);
    ~SimulatorWindow();

public slots:

    void cellOpened();
private:

    Ui::SimulatorWindow *ui;
    QPushButton *** cellGrid;
    int boardWidth;
    int boardHeight;
    Board * board;

};

#endif // SIMULATORWINDOW_H
