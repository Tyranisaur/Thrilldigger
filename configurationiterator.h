#ifndef CONFIGURATIONITERATOR_H
#define CONFIGURATIONITERATOR_H


class ConfigurationIterator
{
public:
    ConfigurationIterator(bool** badSpotArray, int arrayLength, int lowBound, int highBound);
    ~ConfigurationIterator();
    bool hasNext();

    int next();

private:
    bool ** badSpotArray;
    int numConstrainedHoles;
    int* maxIndexArray;
    int* indexArray;
    int arrayLength;
    int numBadSpots;
};

#endif // CONFIGURATIONITERATOR_H
