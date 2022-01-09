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
    ~Solver();

    void setCell(std::size_t x, std::size_t y, DugTypeEnum type);
    const std::vector<double> &getProbabilityArray() const;
    void reload();

    double getTotalNumConfigurations() const;
    uint64_t getIterations() const;
    std::size_t getLegalIterations() const;
    std::size_t getConstrainedHoles() const;
    std::size_t getPartitions() const;

    void partitionCalculate();

signals:
    void done();

private:
    const ProblemParameters params_;
    const std::size_t numHoles = params_.width * params_.height;
    std::size_t bombsAmongConstrainedHoles = 0;
    std::vector<double> probabilities = std::vector<double>(std::size_t(numHoles));
    std::vector<Constraint *> constraintList;
    std::vector<Constraint> constraints = std::vector<Constraint>(std::size_t(numHoles));

    std::vector<Partition> partitions = std::vector<Partition>(std::size_t(numHoles));
    std::vector<bool> badSpots = std::vector<bool>(std::size_t(numHoles), false);

    std::vector<std::unordered_set<Constraint *>> imposingConstraints = std::vector<std::unordered_set<Constraint *>>(std::size_t(numHoles));
    std::unordered_set<std::size_t> constrainedUnopenedHoles;
    std::unordered_set<std::size_t> unconstrainedUnopenedHoles;
    std::unordered_set<std::size_t> knownSafeSpots;
    std::unordered_set<std::size_t> knownBadSpots;
    std::vector<Partition *> partitionList;
    std::vector<Partition *> sunkenPartitions;

    std::vector<DugTypeEnum> board =
        std::vector<DugTypeEnum>(numHoles, dugtype::undug);
    double totalWeight = 0.0;
    std::uint64_t totalIterations = 0;
    std::size_t legalIterations = 0;
    std::size_t numConstrained = 0;

    bool validateBoard() const;
    void setKnownSafeSpot(std::size_t index);
    void setKnownBadSpot(std::size_t index);
    void resetBoard();
    void generatePartitions();
};
