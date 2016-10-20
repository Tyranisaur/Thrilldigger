#include "configurationiterator.h"

ConfigurationIterator::ConfigurationIterator(
        bool ** badSpotArray,
        int arrayLength,
        int lowBound,
        int highBound)
{
    numBadSpots = 0;
    this->badSpotArray = badSpotArray;
    numConstrainedHoles = arrayLength;
    indexArray = new int[highBound];
    maxIndexArray = new int[highBound];
    this->arrayLength = highBound;
    for(int i = highBound - 1; i >= 0; i--)
    {
        indexArray[i] = i;
        maxIndexArray[i] = i < lowBound ?
                    numConstrainedHoles + i - lowBound:
                    numConstrainedHoles;
    }
    for(int i = 0; i < arrayLength; i++)
    {
        if(i < highBound)
        {
            *badSpotArray[i] = true;
            numBadSpots++;
        }
        else
        {
            *badSpotArray[i] = false;
        }
    }

}

ConfigurationIterator::~ConfigurationIterator()
{
    delete indexArray;
    delete maxIndexArray;
}

bool ConfigurationIterator::hasNext()
{
    for(int i = arrayLength - 1; i >= 0; i--)
    {
        if(indexArray[i] < maxIndexArray[i])
        {
            *badSpotArray[indexArray[i]] = false;
            numBadSpots--;
            indexArray[i]++;
            if(indexArray[i] < numConstrainedHoles)
            {
                numBadSpots++;
                *badSpotArray[indexArray[i]] = true;
            }
            for(int j = i + 1; j < arrayLength; j++)
            {
                if(indexArray[j] < numConstrainedHoles)
                {
                    numBadSpots--;
                    *badSpotArray[indexArray[j]] = false;
                }
                indexArray[j] = indexArray[j-1] + 1;
                if(indexArray[j] < numConstrainedHoles)
                {
                    numBadSpots++;
                    *badSpotArray[indexArray[j]] = true;
                }
            }
            return true;
        }

    }
    return false;

}

int ConfigurationIterator::next()
{
    return numBadSpots;
}
