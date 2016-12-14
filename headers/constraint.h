#ifndef CONSTRAINT_H
#define CONSTRAINT_H
#include <vector>
#include <QList>

struct Hole;

struct Constraint{
    int maxBadness;
    QList<int> holes;
};

#endif // CONSTRAINT_H
