#include "headers/simulatorwindow.h"
#include "ui_simulatorwindow.h"
#include "headers/board.h"
#include "headers/problemparameters.h"
#include "headers/DugType.h"
#include <QPushButton>
#include <QCloseEvent>

SimulatorWindow::SimulatorWindow(ProblemParameters * params, QWidget *parent ) :
    QMainWindow(parent),
    ui(new Ui::SimulatorWindow),
    board(params)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QPushButton * button;
    cellGrid = new QPushButton**[params->height] ;
    boardHeight = params->height;
    boardWidth = params->width;
    rupeeTotal = 0;
    for(int y = 0; y < params->height; y++)
    {
        cellGrid[y] = new QPushButton*[params->width];
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
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            delete cellGrid[y][x];
        }
        delete[] cellGrid[y];
    }
    delete[] cellGrid;
}


void SimulatorWindow::cellOpened(int x, int y)
{
    QPushButton * button = cellGrid[y][x];
    DugType::DugType value = board.getCell(x, y);
    switch(value)
    {
    case DugType::DugType::bomb:
        button->setStyleSheet("background: black");
        for(int y = 0; y < boardHeight; y++)
        {
            for(int x = 0; x < boardWidth; x++)
            {
                cellGrid[y][x]->setEnabled(false);
            }
        }
        break;
    case DugType::DugType::rupoor:
        button->setStyleSheet("background: gray");
        rupeeTotal = rupeeTotal - 10 < 0 ? 0 : rupeeTotal - 10;
        break;
    case DugType::DugType::green:
        button->setStyleSheet("background: green");
        rupeeTotal += 1;
        break;
    case DugType::DugType::blue:
        button->setStyleSheet("background: blue");
        rupeeTotal += 5;
        break;
    case DugType::DugType::red:
        button->setStyleSheet("background: red");
        rupeeTotal += 20;
        break;
    case DugType::DugType::silver:
        button->setStyleSheet("background: silver");
        rupeeTotal += 100;
        break;
    case DugType::DugType::gold:
        button->setStyleSheet("background: gold");
        rupeeTotal += 300;
        break;
    }
    if(board.hasWon()){
        for(int y = 0; y < boardHeight; y++)
        {
            for(int x = 0; x < boardWidth; x++)
            {
                cellGrid[y][x]->setEnabled(false);
            }
        }
    }
    QString text = "Rupees: " + QString::number(rupeeTotal);
    ui->scoreLabel->setText(text);
    button->setEnabled(false);
    emit openedCell(x, y, value);
}

void SimulatorWindow::closeEvent(QCloseEvent * e)
{
    emit closing();
    e->accept();
}
