#include "headers/solverwindow.h"

#include "headers/board.h"
#include "headers/dugtype.h"
#include "headers/problemparameters.h"
#include "headers/solver.h"
#include "ui_solverwindow.h"
#include <QCloseEvent>
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QMovie>
#include <QThread>
#include <QVBoxLayout>
#include <cstddef>

SolverWindow::SolverWindow(const ProblemParameters &params, QWidget *parent)
    : QMainWindow(parent),
      ui(std::make_unique<Ui::SolverWindow>()),
      cellGrid(params.height, params.width),
      boardState(params.height, params.width, DugType::undug),
      movie(std::make_unique<QMovie>(":/resources/ajax-loader.gif")),
      solver(params),
      thread(std::make_unique<QThread>()),
      probabilityArray(solver.getProbabilityArray())

{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    solver.moveToThread(thread.get());
    connect(
        thread.get(), SIGNAL(started()), &solver, SLOT(partitionCalculate()));
    connect(&solver, SIGNAL(done()), this, SLOT(processCalculation()));
    boardHeight = params.height;
    boardWidth = params.width;
    numHoles = boardHeight * boardWidth;
    ui->animationLabel->setMovie(movie.get());
    movie->start();
    ui->animationLabel->hide();

    for (int y = 0; y < boardHeight; y++) {
        for (int x = 0; x < boardWidth; x++) {
            auto *vLayout = new QVBoxLayout;

            auto *button = new QPushButton;
            button->setEnabled(false);
            vLayout->addWidget(button);
            auto *menuButton = new QComboBox;

            menuButton->addItem("Undug");
            menuButton->addItem("Bomb");
            menuButton->addItem("Rupoor");
            menuButton->addItem("Green");
            menuButton->addItem("Blue");
            menuButton->addItem("Red");
            menuButton->addItem("Silver");
            menuButton->addItem("Gold");

            connect(menuButton, &QComboBox::currentTextChanged, [=]() {
                this->cellSet(x, y);
            });

            vLayout->addWidget(menuButton);

            ui->gridLayout->addLayout(vLayout, y, x, Qt::AlignHCenter);
            cellGrid.ref(x, y) = vLayout;
        }
    }
}

void SolverWindow::on_calculateButton_clicked()
{
    ui->animationLabel->show();

    thread->start();
}

void SolverWindow::processCalculation()
{
    thread->exit();
    QPushButton *button;
    double lowest = 1.0;
    int index = 0;
    for (int y = 0; y < boardHeight; y++) {
        for (int x = 0; x < boardWidth; x++) {
            if (boardState.at(x, y) == DugType::DugType::undug) {
                button = static_cast<QPushButton *>(
                    cellGrid.ref(x, y)->itemAt(0)->widget()); // NOLINT
                button->setText(
                    QString::number(probabilityArray[index] * 100, 'f', 2) +
                    "% Bad");
                lowest = std::min(lowest, probabilityArray[index]);
                button->setStyleSheet("background: none");
            }
            index++;
        }
    }
    index = 0;
    for (int y = 0; y < boardHeight; y++) {
        for (int x = 0; x < boardWidth; x++) {
            if (boardState.at(x, y) == DugType::DugType::undug) {
                if (probabilityArray[index] == lowest) {
                    button = static_cast<QPushButton *>(
                        cellGrid.ref(x, y)->itemAt(0)->widget()); // NOLINT
                    button->setStyleSheet("background: cyan");
                }
            }
            index++;
        }
    }

    ui->animationLabel->hide();
}

void SolverWindow::cellSet(int x, int y)
{
    auto *menuButton = static_cast<QComboBox *>(
        cellGrid.ref(x, y)->itemAt(1)->widget()); // NOLINT
    QString text = menuButton->currentText();
    auto *button = static_cast<QPushButton *>(
        cellGrid.ref(x, y)->itemAt(0)->widget()); // NOLINT

    if (text == "Undug") {
        button->setStyleSheet("background: none");
        boardState.ref(x, y) = DugType::DugType::undug;
    } else {
        button->setText("");
    }
    if (text == "Bomb") {
        button->setStyleSheet("background: black");
        boardState.ref(x, y) = DugType::DugType::bomb;
    }
    if (text == "Rupoor") {
        button->setStyleSheet("background: gray");
        boardState.ref(x, y) = DugType::DugType::rupoor;
    }
    if (text == "Green") {
        button->setStyleSheet("background: green");
        boardState.ref(x, y) = DugType::DugType::green;
    }
    if (text == "Blue") {
        button->setStyleSheet("background: blue");
        boardState.ref(x, y) = DugType::DugType::blue;
    }
    if (text == "Red") {
        button->setStyleSheet("background: red");
        boardState.ref(x, y) = DugType::DugType::red;
    }
    if (text == "Silver") {
        button->setStyleSheet("background: silver");
        boardState.ref(x, y) = DugType::DugType::silver;
    }
    if (text == "Gold") {
        button->setStyleSheet("background: gold");
        boardState.ref(x, y) = DugType::DugType::gold;
    }
    solver.setCell(x, y, boardState.at(x, y));
}

void SolverWindow::cellOpened(int x, int y, DugType::DugType type)
{
    auto *menuButton = static_cast<QComboBox *>(
        cellGrid.ref(x, y)->itemAt(1)->widget()); // NOLINT
    QString text;
    auto *button = static_cast<QPushButton *>(
        cellGrid.ref(x, y)->itemAt(0)->widget()); // NOLINT
    switch (type) {
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

void SolverWindow::closeEvent(QCloseEvent *e)
{
    emit closing();
    e->accept();
}
