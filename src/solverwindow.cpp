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
      boardWidth{params.width},
      boardHeight{params.height},
      solver(params),
      movie(":/resources/ajax-loader.gif"),
      probabilityArray(solver.getProbabilityArray())

{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    solver.moveToThread(&thread);
    connect(
        &thread, &QThread::started, [this] { solver.partitionCalculate(); });
    connect(&solver, &Solver::done, this, [this] { processCalculation(); });
    connect(ui.calculateButton, &QPushButton::clicked, [this] {
        calculateButton_clicked();
    });
    ui.animationLabel->setMovie(&movie);
    movie.start();
    ui.animationLabel->hide();

    for (std::size_t y = 0; y != boardHeight; ++y) {
        for (std::size_t x = 0; x != boardWidth; ++x) {
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

            connect(menuButton, &QComboBox::currentTextChanged, [x, y, this] {
                cellSet(x, y);
            });

            vLayout->addWidget(menuButton);

            ui.gridLayout->addLayout(vLayout, int(y), int(x), Qt::AlignHCenter);
            cellGrid.ref(x, y) = vLayout;
        }
    }
}

void SolverWindow::calculateButton_clicked()
{
    ui.animationLabel->show();

    thread.start();
}

void SolverWindow::processCalculation()
{
    thread.exit();
    double lowest = 1.0;
    std::size_t index = 0;
    for (std::size_t y = 0; y != boardHeight; ++y) {
        for (std::size_t x = 0; x != boardWidth; ++x) {
            if (boardState.at(x, y) == dugtype::undug) {
                QPushButton *button = getButton(x, y);
                const double probability = probabilityArray[index];
                button->setText(QString::number(probability * 100, 'f', 2) +
                                "% Bad");
                lowest = std::min(lowest, probability);
                button->setStyleSheet("background: none");
            }
            index++;
        }
    }
    index = 0;
    for (std::size_t y = 0; y != boardHeight; ++y) {
        for (std::size_t x = 0; x != boardWidth; ++x) {
            if (boardState.at(x, y) == dugtype::undug) {
                if (probabilityArray[index] == lowest) {
                    QPushButton *button = getButton(x, y);
                    button->setStyleSheet("background: cyan");
                }
            }
            index++;
        }
    }

    ui.animationLabel->hide();
}

void SolverWindow::cellSet(const std::size_t x, const std::size_t y)
{
    QComboBox *menuButton = getMenu(x, y);
    QPushButton *button = getButton(x, y);

    QString text = menuButton->currentText();

    if (text == "Undug") {
        button->setStyleSheet("background: none");
        boardState.ref(x, y) = dugtype::undug;
    } else {
        button->setText("");
    }
    if (text == "Bomb") {
        button->setStyleSheet("background: black");
        boardState.ref(x, y) = dugtype::bomb;
    }
    if (text == "Rupoor") {
        button->setStyleSheet("background: gray");
        boardState.ref(x, y) = dugtype::rupoor;
    }
    if (text == "Green") {
        button->setStyleSheet("background: green");
        boardState.ref(x, y) = dugtype::green;
    }
    if (text == "Blue") {
        button->setStyleSheet("background: blue");
        boardState.ref(x, y) = dugtype::blue;
    }
    if (text == "Red") {
        button->setStyleSheet("background: red");
        boardState.ref(x, y) = dugtype::red;
    }
    if (text == "Silver") {
        button->setStyleSheet("background: silver");
        boardState.ref(x, y) = dugtype::silver;
    }
    if (text == "Gold") {
        button->setStyleSheet("background: gold");
        boardState.ref(x, y) = dugtype::gold;
    }
    solver.setCell(x, y, boardState.at(x, y));
}

void SolverWindow::cellOpened(const std::size_t x,
                              const std::size_t y,
                              DugTypeEnum type)
{
    QComboBox *menuButton = getMenu(x, y);
    QPushButton *button = getButton(x, y);

    QString text;

    switch (type) {
    case dugtype::bomb:
        text = "Bomb";
        button->setStyleSheet("background: black");
        break;
    case dugtype::rupoor:
        text = "Rupoor";
        button->setStyleSheet("background: gray");
        break;
    case dugtype::green:
        text = "Green";
        button->setStyleSheet("background: green");
        break;
    case dugtype::blue:
        text = "Blue";
        button->setStyleSheet("background: blue");
        break;
    case dugtype::red:
        text = "Red";
        button->setStyleSheet("background: red");
        break;
    case dugtype::silver:
        text = "Silver";
        button->setStyleSheet("background: silver");
        break;
    case dugtype::gold:
        text = "Gold";
        button->setStyleSheet("background: gold");
        break;
    case dugtype::undug:
        break;
    }
    menuButton->setCurrentText(std::move(text));
}

QPushButton *SolverWindow::getButton(const std::size_t x, const std::size_t y)
{
    return static_cast<QPushButton *>(
        cellGrid.ref(x, y)->itemAt(0)->widget()); // NOLINT
}

QComboBox *SolverWindow::getMenu(const std::size_t x, const std::size_t y)
{
    return static_cast<QComboBox *>(
        cellGrid.ref(x, y)->itemAt(1)->widget()); // NOLINT
}

void SolverWindow::closeEvent(QCloseEvent *e)
{
    emit closing();
    e->accept();
}
