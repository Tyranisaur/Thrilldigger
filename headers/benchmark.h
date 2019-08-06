#pragma once

#include "board.h"
#include "dugtype.h"
#include "problemparameters.h"
#include "solver.h"
#include <QMap>
#include <QObject>
#include <QThread>
#include <array>

class Benchmark : public QObject
{
    Q_OBJECT
public:
    explicit Benchmark(const ProblemParameters &params);

    void start();

signals:

    void done();

public slots:
    void run();
    void singleRun();

private:
    QMap<double, int> probabilityCount;
    QMap<double, int> probabilityGoneBad;
    QMap<int, int> constrainedHolesEncountered;
    QMap<int, double> constrainedHolesTotalTime;
    QMap<int, int> clicksEncountered;
    QMap<int, double> totalSetupTimeForClicks;
    QMap<int, int> partitionsEncountered;
    QMap<int, double> totalTimeOnPartitions;
    QMap<uint64_t, int> standardIterationsEncountered;
    QMap<uint64_t, int> partitionIterationsEncountered;
    QMap<uint64_t, double> totalTimeOnIterations;
    QMap<int, int> legalIterationsEncountered;
    QMap<int, double> totalTimeOnLegalIterations;

    QMap<int, QList<int> *> constrainedHolesOnClicks;
    QMap<int, QList<int> *> partitionsOnClicks;

    int totalBadSpots;
    int wins;
    uint64_t totalSetupTime;
    uint64_t totalRunTime;
    double totalProbabilities;
    uint64_t totalRupees;
    int totalClicks;
    QThread thread;
    const std::vector<double> *probabilityArray;
    ProblemParameters params;
    Board board;
    Solver solver;
    DugType::DugType **knownBoard;

    std::array<double, 4> ridgepoints = {0.5, 2.5, 4.5, 6.5};
};
