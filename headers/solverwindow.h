#pragma once

#include "solver.h"
#include "ui_solverwindow.h"
#include "vector2d.h"
#include <QMainWindow>
#include <QMovie>
#include <QThread>

class QVBoxLayout;
class QCloseEvent;
class QComboBox;

struct ProblemParameters;

class SolverWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SolverWindow(const ProblemParameters &params,
                          QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *e);

    void cellOpened(std::size_t x, std::size_t y, DugTypeEnum type);

signals:
    void closing();

private:
    Ui::SolverWindow ui{};
    std::size_t boardWidth = 0;
    std::size_t boardHeight = 0;
    std::size_t numHoles = boardWidth * boardHeight;
    Vector2d<QVBoxLayout *> cellGrid{boardHeight, boardWidth};
    Vector2d<DugTypeEnum> boardState{boardHeight, boardWidth, dugtype::undug};
    Solver solver;
    QMovie movie;
    QThread thread;
    const std::vector<double> &probabilityArray;

    QPushButton *getButton(std::size_t x, std::size_t y);
    QComboBox *getMenu(std::size_t x, std::size_t y);

    void calculateButton_clicked();
    void processCalculation();
    void cellSet(std::size_t x, std::size_t y);
};
