#ifndef BENCHMARK_H
#define BENCHMARK_H


#include "dugtype.h"
#include <QObject>
#include <QMap>
#include "problemparameters.h"


class QThread;

class Solver;
class Board;


class Benchmark : public QObject
{
    Q_OBJECT
public:
    explicit Benchmark(ProblemParameters params);
    ~Benchmark();

    void start();



signals:

    void done();

public slots:
    void run();
    void singleRun();

private:
    QMap<double, int> probabilityCount;
    QMap<double, int> probabilityGoneBad;
    int totalBadSpots;
    int wins;
    uint64_t totalSetupTime;
    uint64_t totalRunTime;
    double totalProbabilities;
    uint64_t totalRupees;
    int totalClicks;
    QThread * thread;
    double ** probabilityArray;
    ProblemParameters params;
    Board * board;
    Solver * solver;
    DugType::DugType ** knownBoard;

};

#endif // BENCHMARK_H
