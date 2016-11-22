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
#include <QMovie>
#include <QThread>
#include <QCloseEvent>

SolverWindow::SolverWindow(ProblemParameters * params, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SolverWindow)
{
    thread = new QThread();
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout * vLayout;
    QPushButton * button;
    QComboBox * menuButton;
    solver = new Solver(params);
    probabilityArray = solver->getProbabilityArray();
    solver->moveToThread(thread);
    connect(thread,
            SIGNAL(started()),
            solver,
            SLOT(partitionCalculate()));
    connect(solver,
            SIGNAL(done()),
            this,
            SLOT(processCalculation()));
    boardHeight = params->height;
    boardWidth = params->width;
    cellGrid = new QVBoxLayout **[boardHeight];
    boardState = new DugType::DugType *[boardHeight];
    movie = new QMovie("C:\\Users\\Torgeir\\Documents\\GitHub\\Thrilldigger\\ajax-loader.gif");
    ui->animationLabel->setMovie(movie);
    movie->start();
    ui->animationLabel->hide();

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
    QComboBox * menuButton;
    QPushButton * button;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            menuButton = (QComboBox *)cellGrid[y][x]->itemAt(1)->widget();
            button = (QPushButton *)cellGrid[y][x]->itemAt(0)->widget();
            delete menuButton;
            delete button;
            delete cellGrid[y][x];
        }
        delete[] cellGrid[y];
    }
    delete thread;
    delete ui;
    delete[] cellGrid;
    delete[] boardState;
    delete solver;
    delete movie;
}

void SolverWindow::on_calculateButton_clicked()
{
    ui->animationLabel->show();

    thread->start();

}

void SolverWindow::processCalculation()
{
    thread->exit();
    QPushButton * button;
    double lowest = 1.0;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(boardState[y][x] == DugType::DugType::undug)
            {
                button = (QPushButton *)cellGrid[y][x]->itemAt(0)->widget();
                button->setText(QString::number( probabilityArray[y][x] * 100, 'f', 2) + "% Bad");
                lowest = std::min(lowest, probabilityArray[y][x]);
                button->setStyleSheet("background: none");
            }
        }
    }
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(boardState[y][x] == DugType::DugType::undug)
            {
                if(probabilityArray[y][x] == lowest)
                {
                    button = (QPushButton *)cellGrid[y][x]->itemAt(0)->widget();
                    button->setStyleSheet("background: cyan");
                }
            }
        }
    }

    ui->animationLabel->hide();
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

void SolverWindow::cellOpened(int x, int y, DugType::DugType type)
{
    QComboBox * menuButton = (QComboBox *)cellGrid[y][x]->itemAt(1)->widget();
    QString text;
    QPushButton * button = (QPushButton *)cellGrid[y][x]->itemAt(0)->widget();
    switch(type)
    {
    case DugType::DugType::bomb:
        text = "Bomb";
        button->setStyleSheet("background: black");
        break;
    case DugType::DugType::rupoor:
        text = "Rupoor";
        button->setStyleSheet("background: gray");
        break;
    case DugType::DugType::green:
        text = "Green";
        button->setStyleSheet("background: green");
        break;
    case DugType::DugType::blue:
        text = "Blue";
        button->setStyleSheet("background: blue");
        break;
    case DugType::DugType::red:
        text = "Red";
        button->setStyleSheet("background: red");
        break;
    case DugType::DugType::silver:
        text = "Silver";
        button->setStyleSheet("background: silver");
        break;
    case DugType::DugType::gold:
        text = "Gold";
        button->setStyleSheet("background: gold");
        break;
    }
    menuButton->setCurrentText(text);
}

void SolverWindow::closeEvent(QCloseEvent * e)
{
    emit closing();
    e->accept();
}


