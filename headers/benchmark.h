#pragma once

#include "board.h"
#include "dugtype.h"
#include "problemparameters.h"
#include "solver.h"
#include "vector2d.h"

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
    QMap<double, std::size_t> probabilityCount_;
    QMap<double, std::size_t> probabilityGoneBad_;
    QMap<std::size_t, std::size_t> constrainedHolesEncountered_;
    QMap<std::size_t, std::size_t> constrainedHolesTotalTime_;
    QMap<std::size_t, std::size_t> clicksEncountered_;
    QMap<std::size_t, double> totalSetupTimeForClicks_;
    QMap<std::size_t, std::size_t> partitionsEncountered_;
    QMap<std::size_t, double> totalTimeOnPartitions_;
    QMap<uint64_t, std::size_t> standardIterationsEncountered_;
    QMap<uint64_t, std::size_t> partitionIterationsEncountered_;
    QMap<uint64_t, std::size_t> totalTimeOnIterations_;
    QMap<std::size_t, std::size_t> legalIterationsEncountered_;
    QMap<std::size_t, double> totalTimeOnLegalIterations_;

    QMap<std::size_t, QList<std::size_t> *> constrainedHolesOnClicks_;
    QMap<std::size_t, QList<std::size_t> *> partitionsOnClicks_;

    std::size_t totalBadSpots_ = 0;
    std::size_t wins_ = 0;
    double totalSetupTime_ = 0;
    double totalRunTime_ = 0;
    double totalProbabilities_ = 0.0;
    uint64_t totalRupees_ = 0;
    std::size_t totalClicks_ = 0;
    QThread thread_;
    const std::vector<double> *probabilityArray_ = nullptr;
    ProblemParameters params_;
    Board board_;
    Solver solver_;
    Vector2d<DugTypeEnum> knownBoard_;

    constexpr static std::array<double, 4> ridgepoints_ = {0.5, 2.5, 4.5, 6.5};
};
