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
class QThread;

class Solver : public QObject
{
    Q_OBJECT
public:
    Solver(ProblemParameters * param, QThread* thread);
    ~Solver();

    void setCell(int x, int y, DugType::DugType type);
    double ** getProbabilityArray();

public slots:
    void standardCalculate();
    void partitionCalculate();



private:
    QThread * thread;
    double ** probabilities;
    QList<Constraint*> constraintList;
    Constraint *** constraints;
    bool ** badSpots;
    Hole ** holes;
    QSet<Constraint*> *** imposingConstraints;
    QSet<Hole*> * constrainedUnopenedHoles;
    QSet<Hole*> * unconstrainedUnopenedHoles;
    QSet<Hole*> * knownSafeSpots;
    QSet<Hole*> * knownBadSpots;
    QList<Partition*>* partitionList;

    int bombsAmongConstrainedHoles;
    int boardHeight;
    int boardWidth;
    int bombs;
    int rupoors;
    DugType::DugType ** board;
    bool validateBoard();
    void setKnownSafeSpot(int x, int y);
    void setKnownBadSpot(int x, int y);
    void generatePartitions();
    unsigned long long choose(unsigned long long n, unsigned long long k);
};

#endif // SOLVER_H
