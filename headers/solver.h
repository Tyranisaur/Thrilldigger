#ifndef SOLVER_H
#define SOLVER_H
#include "dugtype.h"
#include <QList>
#include <QSet>
#include <QObject>

struct ProblemParameters;
struct Hole;
struct Constraint;
struct Partition;

class Solver : public QObject
{
    Q_OBJECT
public:
    Solver(ProblemParameters * params);
    ~Solver();

    void setCell(int x, int y, DugType::DugType type);
    double ** getProbabilityArray();
    void reload();

    double getTotalNumConfigurations();
    uint64_t getIterations();
    int getLegalIterations();
    int getConstrainedHoles();
    int getPartitions();

signals:
    void done();

public slots:
    void standardCalculate();
    void partitionCalculate();



private:
    double ** probabilities;
    QList<Constraint*> constraintList;
    Constraint ** constraints;
    bool ** badSpots;
    Hole ** holes;
    QSet<Constraint*> ** imposingConstraints;
    QSet<Hole*>  constrainedUnopenedHoles;
    QSet<Hole*>  unconstrainedUnopenedHoles;
    QSet<Hole*>  knownSafeSpots;
    QSet<Hole*>  knownBadSpots;
    QList<Partition*> partitionList;
    QList<Partition*> sunkenPartitions;

    QSet<Constraint*> emptySet;
    int bombsAmongConstrainedHoles;
    int boardHeight;
    int boardWidth;
    int bombs;
    int rupoors;
    DugType::DugType ** board;
    double totalWeight;
    int legalIterations;
    uint64_t totalIterations;
    int numConstrained;


    bool validateBoard();
    void setKnownSafeSpot(int x, int y);
    void setKnownBadSpot(int x, int y);
    void resetBoard();
    void generatePartitions();
    double choose(unsigned long long n, unsigned long long k);
};

#endif // SOLVER_H
