#ifndef PARTITION_H
#define PARTITION_H
#include <QList>
#include <QSet>

struct Constraint;
struct Hole;

struct Partition{
    QSet<Constraint*> * constraints;
    QList<Hole*> holes;
    char badness;

    bool operator==(Partition & other)
    {
        return *(this->constraints) == *(other.constraints);
    }





};

#endif // PARTITION_H
