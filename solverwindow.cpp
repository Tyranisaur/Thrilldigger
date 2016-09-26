#include "solverwindow.h"
#include "ui_solverwindow.h"
#include "board.h"
#include "problemparameters.h"
#include "dugtype.h"
#include "solver.h"
#include <QVBoxLayout>
#include <QCOmboBox>
#include <QMenu>
#include <QLabel>

SolverWindow::SolverWindow(ProblemParameters * params, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SolverWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout * vLayout;
    QPushButton * button;
    QComboBox * menuButton;
    solver = new Solver(params);
    boardHeight = params->height;
    boardWidth = params->width;
    cellGrid = new QVBoxLayout **[boardHeight];
    boardState = new DugType::DugType *[boardHeight];

    for(int y = 0; y < boardHeight; y++)
    {
        cellGrid[y] = new QVBoxLayout*[boardWidth];
        boardState[y] = new DugType::DugType[boardWidth];
        for(int x = 0; x < boardWidth; x++)
        {
            vLayout = new QVBoxLayout;

            button = new QPushButton;
            button->setEnabled(false);
            vLayout->addWidget(button);
            menuButton = new QComboBox;

            menuButton->addItem("Undug");
            menuButton->addItem("Bomb");
            menuButton->addItem("Rupoor");
            menuButton->addItem("Green");
            menuButton->addItem("Blue");
            menuButton->addItem("Red");
            menuButton->addItem("Silver");
            menuButton->addItem("Gold");


            connect(menuButton,
                    &QComboBox::currentTextChanged,
                    [=]()
            {
                this->cellSet(x, y);
            });

            vLayout->addWidget(menuButton);

            ui->gridLayout->addLayout(vLayout, y, x, Qt::AlignHCenter);
            cellGrid[y][x] = vLayout;
            boardState[y][x] = DugType::DugType::undug;

        }

    }
}

SolverWindow::~SolverWindow()
{
    delete ui;
    delete[] cellGrid;
    delete[] boardState;
    delete solver;
}

void SolverWindow::on_calculateButton_clicked()
{
    double ** result = solver->calculate();
    QPushButton * button;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(boardState[y][x] == DugType::DugType::undug)
            {
                button = (QPushButton *)cellGrid[y][x]->itemAt(0)->widget();
                button->setText(QString("%2").arg(result[y][x] * 100) + "% Bad");
            }
        }
    }
}

void SolverWindow::cellSet(int x, int y)
{
    QComboBox * menuButton = (QComboBox *)cellGrid[y][x]->itemAt(1)->widget();
    QString text = menuButton->currentText();
    QPushButton * button = (QPushButton *)cellGrid[y][x]->itemAt(0)->widget();

    if( text == "Undug")
    {
        button->setStyleSheet("background: none");
        boardState[y][x] = DugType::DugType::undug;
    }
    else{
        button->setText("");
    }
    if( text == "Bomb")
    {
        button->setStyleSheet("background: black");
        boardState[y][x] = DugType::DugType::bomb;
    }
    if( text == "Rupoor")
    {
        button->setStyleSheet("background: gray");
        boardState[y][x] = DugType::DugType::rupoor;
    }
    if( text == "Green")
    {
        button->setStyleSheet("background: green");
        boardState[y][x] = DugType::DugType::green;
    }
    if(text == "Blue")
    {
        button->setStyleSheet("background: blue");
        boardState[y][x] = DugType::DugType::blue;
    }
    if(text == "Red")
    {
        button->setStyleSheet("background: red");
        boardState[y][x] = DugType::DugType::red;
    }
    if(text == "Silver")
    {
        button->setStyleSheet("background: silver");
        boardState[y][x] = DugType::DugType::silver;
    }
    if(text == "Gold")
    {
        button->setStyleSheet("background: gold");
        boardState[y][x] = DugType::DugType::gold;
    }
    solver->setCell(x, y, boardState[y][x]);


}

