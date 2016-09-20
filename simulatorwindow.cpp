#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"
#include "board.h"
#include <QPushButton>

SimulatorWindow::SimulatorWindow(ProblemParameters * params, QWidget *parent ) :
    QMainWindow(parent),
    ui(new Ui::SimulatorWindow)
{
    ui->setupUi(this);
    QPushButton * button;
    cellGrid = new QPushButton**[params->height] ;
    boardHeight = params->height;
    boardWidth = params->width;
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
                    SIGNAL(clicked(bool)),
                    this,
                    SLOT(cellOpened( )));

            ui->gridLayout->addWidget(button, y, x, Qt::AlignHCenter);
        }
    }
}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
    delete[] cellGrid;
}

#include "iostream"
void SimulatorWindow::cellOpened()
{
    QPushButton * source =  (QPushButton*)sender();
    int x, y;
    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            if (source == cellGrid[i][j])
            {
                y = i;
                x = j;
                goto afterLoop;
            }
        }
    }
afterLoop:
    std::cout << "clicked on x = " << x << ", y = " << y << std::endl;

    source->setStyleSheet("background: blue");
    source->setEnabled(false);
}
