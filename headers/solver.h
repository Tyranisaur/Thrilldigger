#pragma once
#include "constraint.h"
#include "dugtype.h"
#include "partition.h"
#include "problemparameters.h"
#include <QObject>
#include <unordered_set>
#include <vector>

class Solver : public QObject
{
    Q_OBJECT
public:
    Solver(const ProblemParameters &params);

    void setCell(int x, int y, DugType::DugType type);
    const std::vector<double> &getProbabilityArray() const;
    void reload();

    double getTotalNumConfigurations();
    uint64_t getIterations();
    int getLegalIterations();
    int getConstrainedHoles();
    int getPartitions();

signals:
    void done();

public slots:
    void partitionCalculate();

private:
    inline static const std::unordered_set<Constraint *> emptySet;
    ProblemParameters params_;
    int numHoles = 0;
    int bombsAmongConstrainedHoles = 0;
    std::vector<double> probabilities;
    std::vector<Constraint *> constraintList;
    std::vector<Constraint> constraints;

    std::vector<Partition> partitions;
    std::vector<bool> badSpots;

    std::vector<std::unordered_set<Constraint *>> imposingConstraints;
    std::unordered_set<int> constrainedUnopenedHoles;
    std::unordered_set<int> unconstrainedUnopenedHoles;
    std::unordered_set<int> knownSafeSpots;
    std::unordered_set<int> knownBadSpots;
    std::vector<Partition *> partitionList;
    std::vector<Partition *> sunkenPartitions;

    std::vector<DugType::DugType> board;
    double totalWeight = 0.0;
    uint64_t totalIterations = 0;
    int legalIterations = 0;
    int numConstrained = 0;

    bool validateBoard();
    void setKnownSafeSpot(int index);
    void setKnownBadSpot(int index);
    void resetBoard();
    void generatePartitions();
    double choose(unsigned long long n, unsigned long long k);
};
