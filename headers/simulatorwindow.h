#pragma once

#include "board.h"
#include "ui_simulatorwindow.h"
#include "vector2d.h"
#include <QMainWindow>

class QPushButton;
class QCloseEvent;

struct ProblemParameters;

class SimulatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulatorWindow(const ProblemParameters &params,
                             QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *e) override;

    void cellOpened(std::size_t x, std::size_t y);

signals:
    void closing();
    void openedCell(std::size_t x, std::size_t y, DugTypeEnum type);

private:
    Ui::SimulatorWindow ui{};
    std::size_t boardWidth = 0;
    std::size_t boardHeight = 0;
    Vector2d<QPushButton *> cellGrid{boardHeight, boardWidth};
    int rupeeTotal = 0;
    Board board;

    void disableAllButtons();
};
