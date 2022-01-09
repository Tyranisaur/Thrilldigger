#include "headers/simulatorwindow.h"

#include "headers/board.h"
#include "headers/dugtype.h"
#include "headers/problemparameters.h"
#include "headers/rupeevalues.h"
#include "ui_simulatorwindow.h"
#include <QCloseEvent>
#include <QPushButton>

SimulatorWindow::SimulatorWindow(const ProblemParameters &params,
                                 QWidget *parent)
    : QMainWindow(parent),
      boardWidth{params.width},
      boardHeight{params.height},
      board(params)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    for (std::size_t y = 0; y != params.height; ++y) {
        for (std::size_t x = 0; x != params.width; ++x) {
            QPushButton *button = new QPushButton(ui.gridLayoutWidget);
            cellGrid.ref(x, y) = button;
            connect(button, &QPushButton::clicked, [x, y, this] {
                this->cellOpened(x, y);
            });

            ui.gridLayout->addWidget(button, int(y), int(x), Qt::AlignHCenter);
        }
    }
}

void SimulatorWindow::cellOpened(std::size_t x, std::size_t y)
{
    QPushButton *button = cellGrid.ref(x, y);
    const DugTypeEnum value = board.getCell(x, y);
    switch (value) {
    case dugtype::bomb:
        button->setStyleSheet("background: black");
        disableAllButtons();
        break;
    case dugtype::rupoor:
        button->setStyleSheet("background: gray");
        break;
    case dugtype::green:
        button->setStyleSheet("background: green");
        break;
    case dugtype::blue:
        button->setStyleSheet("background: blue");
        break;
    case dugtype::red:
        button->setStyleSheet("background: red");
        break;
    case dugtype::silver:
        button->setStyleSheet("background: silver");
        break;
    case dugtype::gold:
        button->setStyleSheet("background: gold");
        break;
    case dugtype::undug:
        break;
    }
    rupeeTotal += rupee::value(value);
    rupeeTotal = std::max(rupeeTotal, 0);
    if (board.hasWon()) {
        disableAllButtons();
    }
    QString text = "Rupees: " + QString::number(rupeeTotal);
    ui.scoreLabel->setText(std::move(text));
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
