#pragma once

#include "board.h"
#include "ui_simulatorwindow.h"
#include "vector2d.h"
#include <QMainWindow>
#include <memory>

class QPushButton;
class QCloseEvent;

struct ProblemParameters;

class SimulatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulatorWindow(const ProblemParameters &params,
                             QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *e);

public slots:

    void cellOpened(int x, int y);

signals:
    void closing();
    void openedCell(int x, int y, DugType::DugType type);

private:
    std::unique_ptr<Ui::SimulatorWindow> ui;
    Vector2d<QPushButton *> cellGrid;
    int boardWidth;
    int boardHeight;
    int rupeeTotal;
    Board board;

    void disableAllButtons();
};
