#ifndef CONSTRAINT_H
#define CONSTRAINT_H
#include <vector>
#include <QList>

struct Hole;

struct Constraint{
    unsigned char x;
    unsigned char y;
    int maxBadness;
    QList<int> holes;
};

#endif // CONSTRAINT_H
