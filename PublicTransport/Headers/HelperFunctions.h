#pragma once
#include "TransportGraph.h"
#include <iostream>

void printShortestTimePath(unsigned shortestTime, unsigned startTime, const std::string& startBusStopName, const std::string& endBusStopName, const std::vector<vertexAndDistancePair>& path);
void printMinimalBusesTakenPath(unsigned minimalBusesTakenCount, const std::vector<vertexAndDistancePair>& path);
void printMinimumTimeSpentPath(unsigned minimumTimeSpent, const std::vector<vertexAndDistancePair>& path);
std::pair<unsigned, unsigned> convertMinutesToClockTime(unsigned totalMinutes);