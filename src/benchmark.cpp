#include "headers/benchmark.h"

#include "headers/board.h"
#include "headers/problemparameters.h"
#include "headers/solver.h"
#include <QThread>
#include <QTime>
#include <iostream>

Benchmark::Benchmark(const ProblemParameters &params)
    : params(params), board(params), solver(params)
{
    moveToThread(&thread);
    //    solver = new Solver*[100];

    //    for(int i = 0; i < 100; i++)
    //    {
    //        solver[i] = new Solver(&params);
    //    }
    connect(&thread, SIGNAL(started()), this, SLOT(run()));
}

void Benchmark::start()
{
    thread.start();
}

void Benchmark::run()
{
    wins = 0;
    totalBadSpots = 0;
    totalProbabilities = 0.0;
    totalRupees = 0;
    totalClicks = 0;
    totalSetupTime = 0;
    totalRunTime = 0;
    knownBoard = new DugType::DugType *[params.height];
    probabilityArray = &solver.getProbabilityArray();
    for (int y = 0; y < params.height; y++) {
        knownBoard[y] = new DugType::DugType[params.width];
    }
    QTime timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
        for (int y = 0; y < params.height; y++) {
            std::fill(knownBoard[y],
                      knownBoard[y] + params.width,
                      DugType::DugType::undug);
        }

        singleRun();
        //        for(int j = 0; j < 100; j++)
        //        {
        //            solver[j]->reload();

        //        }
        solver.reload();
        board.reload();
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

    double numConstrainedAverage;
    double numPartitionsAverage;
    for (int key : clicksEncountered.keys()) {
        numConstrainedAverage = 0.0;
        numPartitionsAverage = 0.0;
        for (int i = 0; i < clicksEncountered.value(key); i++) {
            numConstrainedAverage += constrainedHolesOnClicks.value(key)->at(i);
            numPartitionsAverage += partitionsOnClicks.value(key)->at(i);
        }
        numConstrainedAverage /= clicksEncountered.value(key);
        numPartitionsAverage /= clicksEncountered.value(key);
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
    for (int y = 0; y < params.height; y++) {
        delete[] knownBoard[y];
    }
    delete[] knownBoard;
    thread.exit();
    emit done();
}

void Benchmark::singleRun()
{
    QTime timer;
    DugType::DugType newSpot;
    int index;
    int filterIndex;
    double lowestprobability;
    double highestNeighborSum = 0.0;
    // double ridgedistance;
    double lowestridgedistance;
    double neighborSum;
    int bestX = 0;
    int bestY = 0;
    int sumbadspots = 0;
    double sumProbabilities = 0.0;
    int rupees = 0;
    int clicks = 0;
    double setupTime = 0;
    double runTime = 0;
    // double individualRunTime;
    double individualSetupTime = 0.0;

    int numConstrainedHoles;
    int partitions;
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

    solver.partitionCalculate();

    //    runTime += timer.elapsed() /100.0;
    numConstrainedHoles = solver.getConstrainedHoles();
    partitions = solver.getPartitions();
    //    partitionIterations = solver[0]->getIterations();
    //    partitionIterations = solver[1]->getIterations();
    //    legalIterations = solver[0]->getLegalIterations();

    if (clicksEncountered.contains(clicks)) {
        clicksEncountered.insert(clicks, clicksEncountered.value(clicks) + 1);
    } else {
        clicksEncountered.insert(clicks, 1);
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
    if (constrainedHolesOnClicks.contains(clicks)) {
        constrainedHolesOnClicks.value(clicks)->append(numConstrainedHoles);
        partitionsOnClicks.value(clicks)->append(partitions);
    } else {
        constrainedHolesOnClicks.insert(clicks, new QList<int>);
        partitionsOnClicks.insert(clicks, new QList<int>);
        constrainedHolesOnClicks.value(clicks)->append(numConstrainedHoles);
        partitionsOnClicks.value(clicks)->append(partitions);
    }

    while (!board.hasWon()) {
        lowestprobability = 1.0;
        lowestridgedistance = 1.0;
        index = 0;
        for (int y = 0; y < params.height; y++) {
            for (int x = 0; x < params.width; x++) {
                if (knownBoard[y][x] == DugType::DugType::undug) {
                    if ((*probabilityArray)[index] < lowestprobability) {
                        neighborSum = 0.0;
                        for (int filterY = y - 1; filterY < y + 2; filterY++) {
                            if (filterY >= 0 && filterY < params.height) {
                                for (int filterX = x - 1; filterX < x + 2;
                                     filterX++) {
                                    if (filterX >= 0 &&
                                        filterX < params.width) {
                                        if (filterX != x || filterY != y) {
                                            filterIndex =
                                                filterY * params.width +
                                                filterX;
                                            neighborSum += (*probabilityArray)
                                                [filterIndex];
                                        }
                                    }
                                }
                            }
                        }
                        highestNeighborSum = neighborSum;
                        lowestprobability = (*probabilityArray)[index];
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
                    } else if ((*probabilityArray)[index] ==
                               lowestprobability) {
                        neighborSum = 0.0;
                        for (int filterY = y - 1; filterY < y + 2; filterY++) {
                            if (filterY >= 0 && filterY < params.height) {
                                for (int filterX = x - 1; filterX < x + 2;
                                     filterX++) {
                                    if (filterX >= 0 &&
                                        filterX < params.width) {
                                        if (filterX != x || filterY != y) {
                                            filterIndex =
                                                filterY * params.width +
                                                filterX;
                                            neighborSum += (*probabilityArray)
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
                        if (neighborSum > highestNeighborSum) {
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
        probabilityCount.insert(lowestprobability,
                                probabilityCount.value(lowestprobability) + 1);
        newSpot = board.getCell(bestX, bestY);
        clicks++;
        knownBoard[bestY][bestX] = newSpot;
        if (newSpot == DugType::DugType::bomb) {
            sumbadspots++;
            probabilityGoneBad.insert(
                lowestprobability,
                probabilityGoneBad.value(lowestprobability) + 1);
            break;
        }
        if (newSpot == DugType::DugType::rupoor) {
            sumbadspots++;
            probabilityGoneBad.insert(
                lowestprobability,
                probabilityGoneBad.value(lowestprobability) + 1);
            rupees = std::max(rupees - 10, 0);
        } else if (newSpot == DugType::DugType::green) {
            rupees += 1;
        }

        else if (newSpot == DugType::DugType::blue) {
            rupees += 5;
        } else if (newSpot == DugType::DugType::red) {
            rupees += 20;
        } else if (newSpot == DugType::DugType::silver) {
            rupees += 100;
        } else if (newSpot == DugType::DugType::green) {
            rupees += 300;
        }
        // timer.restart();

        //        for(int i = 0; i < 100; i++)
        //        {
        //            solver[i]->setCell(bestX, bestY, newSpot);
        //        }
        solver.setCell(bestX, bestY, newSpot);
        //        individualSetupTime = timer.elapsed() / 2.0;
        //        setupTime += individualSetupTime;
        timer.restart();
        //        for(int i = 0; i < 100; i++)
        //        {
        //            solver[i]->partitionCalculate();
        //        }
        solver.partitionCalculate();
        //        solver[0]->standardCalculate();
        //        solver[1]->partitionCalculate();
        // individualRunTime = timer.elapsed() / 100.0;
        //        runTime += individualRunTime;

        numConstrainedHoles = solver.getConstrainedHoles();
        partitions = solver.getPartitions();
        // partitionIterations = solver[0]->getIterations();
        //        partitionIterations = solver[1]->getIterations();

        // std::cout << clicks << "\t " << numConstrainedHoles  << "\t" <<
        // partitions << std::endl;

        //        legalIterations = solver[0]->getLegalIterations();

        if (clicksEncountered.contains(clicks)) {
            clicksEncountered.insert(clicks,
                                     clicksEncountered.value(clicks) + 1);
            totalSetupTimeForClicks.insert(
                clicks,
                totalSetupTimeForClicks.value(clicks) + individualSetupTime);
        } else {
            clicksEncountered.insert(clicks, 1);
            totalSetupTimeForClicks.insert(clicks, individualSetupTime);
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
        if (constrainedHolesOnClicks.contains(clicks)) {
            constrainedHolesOnClicks.value(clicks)->append(numConstrainedHoles);
            partitionsOnClicks.value(clicks)->append(partitions);
        } else {
            constrainedHolesOnClicks.insert(clicks, new QList<int>);
            partitionsOnClicks.insert(clicks, new QList<int>);
            constrainedHolesOnClicks.value(clicks)->append(numConstrainedHoles);
            partitionsOnClicks.value(clicks)->append(partitions);
        }
    }
    if (board.hasWon()) {
        wins++;
    }
    totalClicks += clicks;
    totalBadSpots += sumbadspots;
    totalProbabilities += sumProbabilities;
    totalRupees += rupees;
    totalRunTime += runTime;
    totalSetupTime += setupTime;

    // std::cout << sumbadspots << "\t" << sumProbabilities << std::endl;
    //    std::cout << clicks << "\t" << runTime << std::endl;
}
