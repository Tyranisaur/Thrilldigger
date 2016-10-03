#ifndef CONSTRAINT_H
#define CONSTRAINT_H
#include <vector>
#include <QList>

struct Hole;

struct Constraint{
    int x;
    int y;
    int maxBadness;
    QList<Hole*> holes;
};

#endif // CONSTRAINT_H
