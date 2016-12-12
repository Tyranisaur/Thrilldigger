#ifndef SOLVERWINDOW_H
#define SOLVERWINDOW_H

#include <QMainWindow>
#include "solver.h"

namespace Ui {
    class SolverWindow;
}

namespace DugType {
    enum DugType;
}

class QVBoxLayout;
class QThread;
class QCloseEvent;
class QMovie;

struct ProblemParameters;


class SolverWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SolverWindow(ProblemParameters * params, QWidget *parent = 0);
    ~SolverWindow();
    void closeEvent(QCloseEvent * e);

private slots:
    void on_calculateButton_clicked();
    void processCalculation();
    void cellSet(int x, int y);
    void cellOpened(int x, int y, DugType::DugType type);

signals:
    void closing();

private:
    Ui::SolverWindow *ui;
    QVBoxLayout *** cellGrid;
    QMovie * movie;
    DugType::DugType ** boardState;
    Solver solver;
    QThread * thread;
    double * probabilityArray;
    int boardWidth;
    int boardHeight;
    int numHoles;

};

#endif // SOLVERWINDOW_H
