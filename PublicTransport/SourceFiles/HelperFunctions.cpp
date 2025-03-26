#include "HelperFunctions.h"

void printShortestTimePath(unsigned shortestTime, unsigned startTime, const std::string& startBusStopName, const std::string& endBusStopName, const std::vector<vertexAndDistancePair>& path)
{
    if (shortestTime == INT_MAX)
    {
        std::cout << "No path between " << startBusStopName << " and " << endBusStopName << "." << std::endl;
        return;
    }

    std::pair<unsigned, unsigned> currentClockTime = convertMinutesToClockTime(path[1].currentTime);
    std::cout << "From Bus Stop " << startBusStopName << " take bus number " << path[1].busTakenToVertex << " at ";
    if (currentClockTime.second < 10)
    {
        std::cout << currentClockTime.first << ":" << "0" << currentClockTime.second << " ";
    }
    else
    {
        std::cout << currentClockTime.first << ":" << currentClockTime.second << " ";
    }

    for (size_t i = 2; i < path.size(); i++)
    {
        if (path[i - 1].busTakenToVertex == path[i].busTakenToVertex)
        {
            continue;
        }

        std::cout << "to ";
        currentClockTime = convertMinutesToClockTime(path[i].currentTime);
        std::cout << path[i - 1].vertex << ", then take bus number " << path[i].busTakenToVertex << " at ";
        if (currentClockTime.second < 10)
        {
            std::cout << currentClockTime.first << ":" << "0" << currentClockTime.second << " to ";
        }
        else
        {
            std::cout << currentClockTime.first << ":" << currentClockTime.second << " to ";
        }
        
    }
    currentClockTime = convertMinutesToClockTime(startTime + shortestTime);
    std::cout << path[path.size() - 1].vertex << ". You have reached your destination at "; // << startTime + shortestTime << "! Time traveled: " << shortestTime << " minutes!";
    if (currentClockTime.second < 10)
    {
        std::cout << currentClockTime.first << ":" << "0" << currentClockTime.second;
    }
    else
    {
        std::cout << currentClockTime.first << ":" << currentClockTime.second;
    }
    std::cout << "! Time traveled: " << shortestTime << " minutes!";
    
    std::cout << std::endl;
}

void printMinimalBusesTakenPath(unsigned minimalBusesTakenCount, const std::vector<vertexAndDistancePair>& path)
{
    if (minimalBusesTakenCount == INT_MAX)
    {
        std::cout << "No path found!" << std::endl;
        return;
    }
    
    if (minimalBusesTakenCount != 1)
    {
        std::cout << "From " << path[0].vertex << " with " << path[1].busTakenToVertex << " ";
        for (size_t i = 2; i < path.size(); i++)
        {
            if (path[i - 1].busTakenToVertex == path[i].busTakenToVertex)
            {
                continue;
            }   
            std::cout << "to ";
            std::cout << path[i - 1].vertex << ", then take bus number " << path[i].busTakenToVertex << " to ";
                
        }
        std::cout << path[path.size() - 1].vertex << ". You have reached your destination!";
        std::cout << " Number of transfers: " << minimalBusesTakenCount - 1 << ".";
        std::cout << std::endl;
        return;
    }
    std::cout << "From " << path[0].vertex << " with " << path[1].busTakenToVertex << " to ";
    std::cout << path[path.size() - 1].vertex << ". You have reached your destination!";
    std::cout << " Buses Taken: " << minimalBusesTakenCount << ".";
    
    std::cout << std::endl;
}

void printMinimumTimeSpentPath(unsigned minimumTimeSpent, const std::vector<vertexAndDistancePair>& path)
{
    if (minimumTimeSpent == INT_MAX)
    {
        std::cout << "No path between " << path[0].vertex << " and " << path[path.size() - 1].vertex << "." << std::endl;
        return;
    }
    
    std::cout << "From Bus Stop " << path[0].vertex << " take bus number " << path[1].busTakenToVertex << " ";

    for (size_t i = 2; i < path.size(); i++)
    {
        if (path[i - 1].busTakenToVertex == path[i].busTakenToVertex)
        {
            continue;
        }
           
        std::cout << "to ";
        std::cout << path[i - 1].vertex << ", then take bus number " << path[i].busTakenToVertex << " to ";      
    }
    std::cout << path[path.size() - 1].vertex << ". You have reached your destination! ";
    std::cout << "Time outside vehicle: " << minimumTimeSpent << " minutes!";
    
    std::cout << std::endl;
}

std::pair<unsigned, unsigned> convertMinutesToClockTime(unsigned totalMinutes)
{
    unsigned hours = totalMinutes / 60;
    unsigned minutes = totalMinutes % 60;
    return { hours, minutes };
}
