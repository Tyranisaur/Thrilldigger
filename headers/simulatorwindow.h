#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QMainWindow>
#include "board.h"

namespace Ui {
class SimulatorWindow;
}
namespace DugType {
enum DugType;
}
class QPushButton;
class QCloseEvent;

struct ProblemParameters;


class SimulatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulatorWindow(ProblemParameters * params, QWidget *parent = 0);
    ~SimulatorWindow();
    void closeEvent(QCloseEvent * e);
    int numRupees();

public slots:

    void cellOpened(int x, int y);

signals:
    void closing();
    void openedCell(int x, int y, DugType::DugType type);

private:

    Ui::SimulatorWindow *ui;
    QPushButton *** cellGrid;
    int boardWidth;
    int boardHeight;
    int rupeeTotal;
    Board board;

};

#endif // SIMULATORWINDOW_H
