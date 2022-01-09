#include "headers/benchmark.h"

#include "headers/board.h"
#include "headers/problemparameters.h"
#include "headers/rupeevalues.h"
#include "headers/solver.h"
#include <QThread>
#include <QTime>
#include <iostream>

Benchmark::Benchmark(const ProblemParameters &params)
    : params_(params), board_(params), solver_(params)
{
    moveToThread(&thread_);
    //    solver = new Solver*[100];

    //    for(int i = 0; i < 100; i++)
    //    {
    //        solver[i] = new Solver(&params);
    //    }
    connect(&thread_, &QThread::started, [this] { run(); });
}

void Benchmark::start()
{
    thread_.start();
}

void Benchmark::run()
{
    wins_ = 0;
    totalBadSpots_ = 0;
    totalProbabilities_ = 0.0;
    totalRupees_ = 0;
    totalClicks_ = 0;
    totalSetupTime_ = 0;
    totalRunTime_ = 0;
    knownBoard_.reset(params_.width, params_.height, dugtype::undug);
    probabilityArray_ = &solver_.getProbabilityArray();

    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++)
    {

        singleRun();
        //        for(int j = 0; j < 100; j++)
        //        {
        //            solver[j]->reload();

        //        }
        solver_.reload();
        board_.reload();
    }
    //    std::cout << timer.elapsed() << std::endl;
    //        std::cout << totalBadSpots << "\t" << totalProbabilities <<
    //        std::endl;
    //    std::cout << wins / 100000.0  << std::endl;
    //        for(double key: probabilityCount.keys())
    //        {
    //            std::cout <<
    //                         key <<
    //                         "\t" <<
    //                         probabilityCount.value(key) <<
    //                         "\t" <<
    //                         probabilityGoneBad.value(key) <<
    //                         std::endl;
    //        }

    for (const std::size_t key : clicksEncountered_.keys())
    {
        double numConstrainedAverage = 0.0;
        double numPartitionsAverage = 0.0;
        const std::size_t count = clicksEncountered_.value(key);
        for (std::size_t i = 0; i != count; ++i)
        {
            numConstrainedAverage +=
                double(constrainedHolesOnClicks_.value(key)->at(qsizetype(i)));
            numPartitionsAverage +=
                double(partitionsOnClicks_.value(key)->at(qsizetype(i)));
        }
        if (count > 0)
        {
            numConstrainedAverage /= double(count);
            numPartitionsAverage /= double(count);
        }
        std::cout << key << "\t" << numConstrainedAverage << "\t"
                  << numPartitionsAverage << std::endl;
    }

    //    for(uint64_t key: partitionIterationsEncountered.keys())
    //    {
    //        std::cout <<
    //                     key <<
    //                     "\t"   <<
    //                     partitionIterationsEncountered.value(key) <<
    //                     "\t" <<
    //                     totalTimeOnIterations.value(key) /
    //                     partitionIterationsEncountered.value(key) <<
    //                     std::endl;
    //    }

    thread_.exit();
    emit done();
}

void Benchmark::singleRun()
{
    QElapsedTimer timer;
    DugTypeEnum newSpot;
    std::size_t index;
    std::size_t filterIndex;
    double lowestprobability;
    double highestNeighborSum = 0.0;
    // double ridgedistance;
    // double lowestridgedistance;
    double neighborSum;
    std::size_t bestX = 0;
    std::size_t bestY = 0;
    std::size_t sumbadspots = 0;
    double sumProbabilities = 0.0;
    std::uint64_t rupees = 0;
    std::size_t clicks = 0;
    double setupTime = 0;
    double runTime = 0;
    // double individualRunTime;
    double individualSetupTime = 0.0;

    std::size_t numConstrainedHoles;
    std::size_t partitions;
    // uint64_t standardIterations;
    // uint64_t partitionIterations;
    // int legalIterations;

    timer.start();
    //    for(int i = 0; i < 100; i++)
    //    {
    //        solver[i]->partitionCalculate();
    //    }

    //    solver[0]->standardCalculate();
    //    solver[1]->partitionCalculate();

    solver_.partitionCalculate();

    //    runTime += timer.elapsed() /100.0;
    numConstrainedHoles = solver_.getConstrainedHoles();
    partitions = solver_.getPartitions();
    //    partitionIterations = solver[0]->getIterations();
    //    partitionIterations = solver[1]->getIterations();
    //    legalIterations = solver[0]->getLegalIterations();

    if (clicksEncountered_.contains(clicks))
    {
        clicksEncountered_.insert(clicks, clicksEncountered_.value(clicks) + 1);
    }
    else
    {
        clicksEncountered_.insert(clicks, 1);
    }
    //        if(standardIterationsEncountered.contains(standardIterations))
    //        {
    //            standardIterationsEncountered.insert(standardIterations,
    //            standardIterationsEncountered.value(standardIterations) + 1);
    //            totalTimeOnIterations.insert(standardIterations,
    //            totalTimeOnIterations.value(standardIterations) + runTime);
    //        }
    //        else
    //        {
    //            standardIterationsEncountered.insert(standardIterations, 1);
    //            totalTimeOnIterations.insert(standardIterations, runTime);
    //        }
    //    if(partitionIterationsEncountered.contains(partitionIterations))
    //    {
    //        partitionIterationsEncountered.insert(partitionIterations,
    //        partitionIterationsEncountered.value(partitionIterations) + 1);
    //        totalTimeOnIterations.insert(partitionIterations,
    //        totalTimeOnIterations.value(standardIterations) + runTime);
    //    }
    //    else
    //    {
    //        partitionIterationsEncountered.insert(partitionIterations, 1);
    //        totalTimeOnIterations.insert(partitionIterations, runTime);
    //    }

    //    if(constrainedHolesEncountered.contains(numConstrainedHoles))
    //    {
    //        constrainedHolesEncountered.insert(numConstrainedHoles,
    //        constrainedHolesEncountered.value(numConstrainedHoles) + 1);
    //        constrainedHolesTotalTime.insert(numConstrainedHoles,
    //        constrainedHolesTotalTime.value(numConstrainedHoles) + runTime);
    //    }
    //    else
    //    {
    //        constrainedHolesEncountered.insert(numConstrainedHoles, 1);
    //        constrainedHolesTotalTime.insert(numConstrainedHoles, runTime);
    //    }
    //    if(partitionsEncountered.contains(partitions))
    //    {
    //        partitionsEncountered.insert(partitions,
    //        partitionsEncountered.value(partitions) + 1);
    //        totalTimeOnPartitions.insert(partitions,
    //        totalTimeOnPartitions.value(partitions) + runTime);
    //    }
    //    else
    //    {
    //        partitionsEncountered.insert(partitions,
    //        partitionsEncountered.value(partitions) + 1);
    //        totalTimeOnPartitions.insert(partitions,
    //        totalTimeOnPartitions.value(partitions) + runTime);
    //    }
    if (constrainedHolesOnClicks_.contains(clicks))
    {
        constrainedHolesOnClicks_.value(clicks)->append(numConstrainedHoles);
        partitionsOnClicks_.value(clicks)->append(partitions);
    }
    else
    {
        constrainedHolesOnClicks_.insert(clicks, new QList<std::size_t>);
        partitionsOnClicks_.insert(clicks, new QList<std::size_t>);
        constrainedHolesOnClicks_.value(clicks)->append(numConstrainedHoles);
        partitionsOnClicks_.value(clicks)->append(partitions);
    }

    while (!board_.hasWon())
    {
        lowestprobability = 1.0;
        // lowestridgedistance = 1.0;
        index = 0;
        for (std::size_t y = 0; y != params_.height; ++y)
        {
            for (std::size_t x = 0; x != params_.width; ++x)
            {
                if (knownBoard_.at(x, y) == dugtype::undug)
                {
                    if ((*probabilityArray_)[index] < lowestprobability)
                    {
                        neighborSum = 0.0;
                        for (int filterY = int(y) - 1; filterY < int(y) + 2;
                             filterY++)
                        {
                            if (filterY >= 0 &&
                                std::size_t(filterY) < params_.height)
                            {
                                for (int filterX = int(x) - 1;
                                     filterX < int(x) + 2;
                                     filterX++)
                                {
                                    if (filterX >= 0 &&
                                        std::size_t(filterX) < params_.width)
                                    {
                                        if (std::size_t(filterX) != x ||
                                            std::size_t(filterY) != y)
                                        {
                                            filterIndex = std::size_t(filterY) *
                                                              params_.width +
                                                          std::size_t(filterX);
                                            neighborSum += (*probabilityArray_)
                                                [filterIndex];
                                        }
                                    }
                                }
                            }
                        }
                        highestNeighborSum = neighborSum;
                        lowestprobability = (*probabilityArray_)[index];
                        bestX = x;
                        bestY = y;

                        //                        lowestridgedistance = 1.0;
                        //                        for(int i = 0; i < 4; i++)
                        //                        {
                        //                            if(std::abs(neighborSum -
                        //                            ridgepoints[i]) <
                        //                            lowestridgedistance)
                        //                            {
                        //                                lowestridgedistance =
                        //                                std::abs(neighborSum -
                        //                                ridgepoints[i]);
                        //                            }
                        //                        }
                    }
                    else if ((*probabilityArray_)[index] == lowestprobability)
                    {
                        neighborSum = 0.0;
                        for (int filterY = int(y) - 1; filterY < int(y) + 2;
                             filterY++)
                        {
                            if (filterY >= 0 &&
                                std::size_t(filterY) < params_.height)
                            {
                                for (int filterX = int(x) - 1;
                                     filterX < int(x) + 2;
                                     filterX++)
                                {
                                    if (filterX >= 0 &&
                                        std::size_t(filterX) < params_.width)
                                    {
                                        if (std::size_t(filterX) != x ||
                                            std::size_t(filterY) != y)
                                        {
                                            filterIndex = std::size_t(filterY) *
                                                              params_.width +
                                                          std::size_t(filterX);
                                            neighborSum += (*probabilityArray_)
                                                [filterIndex];
                                        }
                                    }
                                }
                            }
                        }

                        //                        ridgedistance = 1.0;
                        //                        for(int i = 0; i < 4; i++)
                        //                        {
                        //                            if(std::abs(neighborSum -
                        //                            ridgepoints[i]) <
                        //                            ridgedistance)
                        //                            {
                        //                                ridgedistance =
                        //                                std::abs(neighborSum -
                        //                                ridgepoints[i]);
                        //                            }
                        //                        }
                        //                        if(ridgedistance <
                        //                        lowestridgedistance)
                        //                        {
                        //                            bestX = x;
                        //                            bestY = y;
                        //                            lowestridgedistance =
                        //                            ridgedistance;
                        //                        }
                        if (neighborSum > highestNeighborSum)
                        {
                            bestX = x;
                            bestY = y;
                            highestNeighborSum = neighborSum;
                        }
                    }
                }
                index++;
            }
        }
        sumProbabilities += lowestprobability;
        //        if(!probabilityCount.contains(lowestprobability))
        //        {
        //            probabilityCount.insert(lowestprobability, 0);
        //            probabilityGoneBad.insert(lowestprobability, 0);
        //        }
        probabilityCount_.insert(
            lowestprobability, probabilityCount_.value(lowestprobability) + 1);
        newSpot = board_.getCell(bestX, bestY);
        clicks++;
        knownBoard_.ref(bestX, bestY) = newSpot;
        if (newSpot == dugtype::bomb || newSpot == dugtype::rupoor)
        {
            sumbadspots++;
            probabilityGoneBad_.insert(
                lowestprobability,
                probabilityGoneBad_.value(lowestprobability) + 1);
            if (newSpot == dugtype::bomb) break;
        }
        rupees += std::uint64_t(rupee::value(newSpot));
        rupees = std::max<std::uint64_t>(rupees, 0);
        // timer.restart();

        //        for(int i = 0; i < 100; i++)
        //        {
        //            solver[i]->setCell(bestX, bestY, newSpot);
        //        }
        solver_.setCell(bestX, bestY, newSpot);
        //        individualSetupTime = timer.elapsed() / 2.0;
        //        setupTime += individualSetupTime;
        timer.restart();
        //        for(int i = 0; i < 100; i++)
        //        {
        //            solver[i]->partitionCalculate();
        //        }
        solver_.partitionCalculate();
        //        solver[0]->standardCalculate();
        //        solver[1]->partitionCalculate();
        // individualRunTime = timer.elapsed() / 100.0;
        //        runTime += individualRunTime;

        numConstrainedHoles = solver_.getConstrainedHoles();
        partitions = solver_.getPartitions();
        // partitionIterations = solver[0]->getIterations();
        //        partitionIterations = solver[1]->getIterations();

        // std::cout << clicks << "\t " << numConstrainedHoles  << "\t" <<
        // partitions << std::endl;

        //        legalIterations = solver[0]->getLegalIterations();

        if (clicksEncountered_.contains(clicks))
        {
            clicksEncountered_.insert(clicks,
                                      clicksEncountered_.value(clicks) + 1);
            totalSetupTimeForClicks_.insert(
                clicks,
                totalSetupTimeForClicks_.value(clicks) + individualSetupTime);
        }
        else
        {
            clicksEncountered_.insert(clicks, 1);
            totalSetupTimeForClicks_.insert(clicks, individualSetupTime);
        }
        //                if(standardIterationsEncountered.contains(standardIterations))
        //                {
        //                    standardIterationsEncountered.insert(standardIterations,
        //                    standardIterationsEncountered.value(standardIterations)
        //                    + 1);
        //                    totalTimeOnIterations.insert(standardIterations,
        //                    totalTimeOnIterations.value(standardIterations) +
        //                    individualRunTime);
        //                }
        //                else
        //                {
        //                    standardIterationsEncountered.insert(standardIterations,
        //                    1);
        //                    totalTimeOnIterations.insert(standardIterations,
        //                    individualRunTime);
        //                }
        //        if(partitionIterationsEncountered.contains(partitionIterations))
        //        {
        //            partitionIterationsEncountered.insert(partitionIterations,
        //            partitionIterationsEncountered.value(partitionIterations)
        //            + 1); totalTimeOnIterations.insert(partitionIterations,
        //            totalTimeOnIterations.value(partitionIterations) +
        //            individualRunTime);
        //        }
        //        else
        //        {
        //            partitionIterationsEncountered.insert(partitionIterations,
        //            1); totalTimeOnIterations.insert(partitionIterations,
        //            individualRunTime);
        //        }

        //        if(constrainedHolesEncountered.contains(numConstrainedHoles))
        //        {
        //            constrainedHolesEncountered.insert(numConstrainedHoles,
        //            constrainedHolesEncountered.value(numConstrainedHoles) +
        //            1); constrainedHolesTotalTime.insert(numConstrainedHoles,
        //            constrainedHolesTotalTime.value(numConstrainedHoles) +
        //            individualRunTime);
        //        }
        //        else
        //        {
        //            constrainedHolesEncountered.insert(numConstrainedHoles,
        //            1); constrainedHolesTotalTime.insert(numConstrainedHoles,
        //            individualRunTime);
        //        }
        //        if(partitionsEncountered.contains(partitions))
        //        {
        //            partitionsEncountered.insert(partitions,
        //            partitionsEncountered.value(partitions) + 1);
        //            totalTimeOnPartitions.insert(partitions,
        //            totalTimeOnPartitions.value(partitions) +
        //            individualRunTime);
        //        }
        //        else
        //        {
        //            partitionsEncountered.insert(partitions, 1);
        //            totalTimeOnPartitions.insert(partitions,
        //            individualRunTime);
        //        }
        if (constrainedHolesOnClicks_.contains(clicks))
        {
            constrainedHolesOnClicks_.value(clicks)->append(
                numConstrainedHoles);
            partitionsOnClicks_.value(clicks)->append(partitions);
        }
        else
        {
            constrainedHolesOnClicks_.insert(clicks, new QList<std::size_t>);
            partitionsOnClicks_.insert(clicks, new QList<std::size_t>);
            constrainedHolesOnClicks_.value(clicks)->append(
                numConstrainedHoles);
            partitionsOnClicks_.value(clicks)->append(partitions);
        }
    }
    if (board_.hasWon())
    {
        wins_++;
    }
    totalClicks_ += clicks;
    totalBadSpots_ += sumbadspots;
    totalProbabilities_ += sumProbabilities;
    totalRupees_ += rupees;
    totalRunTime_ += runTime;
    totalSetupTime_ += setupTime;

    // std::cout << sumbadspots << "\t" << sumProbabilities << std::endl;
    //    std::cout << clicks << "\t" << runTime << std::endl;
}
