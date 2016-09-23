#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"
#include "board.h"
#include <QPushButton>

SimulatorWindow::SimulatorWindow(ProblemParameters * params, QWidget *parent ) :
    QMainWindow(parent),
    ui(new Ui::SimulatorWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QPushButton * button;
    cellGrid = new QPushButton**[params->height] ;
    boardHeight = params->height;
    boardWidth = params->width;
    board = new Board(params);
    for(int y = 0; y < params->height; y++)
    {
        cellGrid[y] = new QPushButton*[params->width];
    }
    for(int y = 0; y < params->height; y++)
    {
        for(int x = 0; x < params->width; x++)
        {
            button = new QPushButton(ui->gridLayoutWidget);
            cellGrid[y][x] = button;
            connect(button,
                    &QPushButton::clicked,
                    [=]()
                    {
                        this->cellOpened(x, y);
                    });

            ui->gridLayout->addWidget(button, y, x, Qt::AlignHCenter);
        }
    }

}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
    delete board;
    delete[] cellGrid;
}


void SimulatorWindow::cellOpened(int x, int y)
{
    QPushButton * button = cellGrid[y][x];
    CellType value = board->getCell(x, y);
    switch(value)
    {
    case CellType::bomb:
        button->setStyleSheet("background: black");
        break;
    case CellType::rupoor:
        button->setStyleSheet("background: gray");
        break;
    case CellType::green:
        button->setStyleSheet("background: green");
        break;
    case CellType::blue:
        button->setStyleSheet("background: blue");
        break;
    case CellType::red:
        button->setStyleSheet("background: red");
        break;
    case CellType::silver:
        button->setStyleSheet("background: silver");
        break;
    case CellType::gold:
        button->setStyleSheet("background: gold");
        break;
    }
    button->setEnabled(false);
}
