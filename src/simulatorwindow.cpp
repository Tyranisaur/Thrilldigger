#include "headers/simulatorwindow.h"

#include "headers/board.h"
#include "headers/dugtype.h"
#include "headers/problemparameters.h"
#include "ui_simulatorwindow.h"
#include <QCloseEvent>
#include <QPushButton>
#include <memory>

SimulatorWindow::SimulatorWindow(const ProblemParameters &params,
                                 QWidget *parent)
    : QMainWindow(parent),
      ui(std::make_unique<Ui::SimulatorWindow>()),
      board(params),
      cellGrid(params.height, params.width)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QPushButton *button;
    boardHeight = params.height;
    boardWidth = params.width;
    rupeeTotal = 0;
    for (int y = 0; y < params.height; y++) {
        for (int x = 0; x < params.width; x++) {
            button = new QPushButton(ui->gridLayoutWidget);
            cellGrid.ref(x, y) = button;
            connect(button, &QPushButton::clicked, [=]() {
                this->cellOpened(x, y);
            });

            ui->gridLayout->addWidget(button, y, x, Qt::AlignHCenter);
        }
    }
}

void SimulatorWindow::cellOpened(int x, int y)
{
    QPushButton *button = cellGrid.ref(x, y);
    DugType::DugType value = board.getCell(x, y);
    switch (value) {
    case DugType::DugType::bomb:
        button->setStyleSheet("background: black");
        disableAllButtons();
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
    if (board.hasWon()) {
        disableAllButtons();
    }
    QString text = "Rupees: " + QString::number(rupeeTotal);
    ui->scoreLabel->setText(text);
    button->setEnabled(false);
    emit openedCell(x, y, value);
}

void SimulatorWindow::closeEvent(QCloseEvent *e)
{
    emit closing();
    e->accept();
}

void SimulatorWindow::disableAllButtons()
{
    for (QPushButton *button : cellGrid) {
        button->setEnabled(false);
    }
}