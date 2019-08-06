#pragma once

#include "solver.h"
#include "ui_solverwindow.h"
#include "vector2d.h"
#include <QMainWindow>
#include <qmovie.h>
#include <qthread.h>

namespace DugType
{
enum DugType : int;
}

class QVBoxLayout;
class QCloseEvent;

struct ProblemParameters;

class SolverWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SolverWindow(const ProblemParameters &params,
                          QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *e);

private slots:
    void on_calculateButton_clicked();
    void processCalculation();
    void cellSet(int x, int y);
    void cellOpened(int x, int y, DugType::DugType type);

signals:
    void closing();

private:
    std::unique_ptr<Ui::SolverWindow> ui;
    Vector2d<QVBoxLayout *> cellGrid;
    std::unique_ptr<QMovie> movie;
    Vector2d<DugType::DugType> boardState;
    Solver solver;
    std::unique_ptr<QThread> thread;
    const std::vector<double> &probabilityArray;
    int boardWidth;
    int boardHeight;
    int numHoles;
};
