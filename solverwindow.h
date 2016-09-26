#ifndef SOLVERWINDOW_H
#define SOLVERWINDOW_H

#include <QMainWindow>

namespace Ui {
    class SolverWindow;
}

namespace DugType {
    enum DugType;
}

class QVBoxLayout;
class Solver;

struct ProblemParameters;


class SolverWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SolverWindow(ProblemParameters * params, QWidget *parent = 0);
    ~SolverWindow();

private slots:
    void on_calculateButton_clicked();
    void cellSet(int x, int y);

private:
    Ui::SolverWindow *ui;
    QVBoxLayout *** cellGrid;
    DugType::DugType ** boardState;
    Solver * solver;
    int boardWidth;
    int boardHeight;

};

#endif // SOLVERWINDOW_H
