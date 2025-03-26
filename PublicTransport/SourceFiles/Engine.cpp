#include "Engine.h"

void Engine::run()
{
    // Names of the needed files to run the project.
	const std::string busesWithStopsFileName = "BusesWithStops.csv";
	const std::string timeTableFileName = "TimeTable.csv";

    // Creating the transport grid.
	TransportGraph transportGrid(busesWithStopsFileName, timeTableFileName);

    // Information needed from the user.
    std::string startBusStopName;
    std::string endBusStopName;
    unsigned startTime;
    std::vector<vertexAndDistancePair> shortestTimePath;
    std::vector<vertexAndDistancePair> minimalBusesTakenPath;
    std::vector<vertexAndDistancePair> minimumTimeSpentPath;

    std::cout << "Enter the name of the bus stop you are currently at: ";
    std::getline(std::cin, startBusStopName);

    while (!transportGrid.checkBusStopExistence(startBusStopName))
    {
        std::cout << "Such bus stop doesn't exist!\n";
        std::cout << "Enter the name of the bus stop you are currently at again: ";
        std::getline(std::cin, startBusStopName);
    }

    std::cout << "Enter the name of the bus stop you want to reach: ";
    std::getline(std::cin, endBusStopName);

    while (!transportGrid.checkBusStopExistence(endBusStopName))
    {
        std::cout << "Such bus stop doesn't exist!\n";
        std::cout << "Enter the name of the bus stop you want to reach again: ";
        std::getline(std::cin, endBusStopName);
    }

    std::cout << "Enter the starting time of your trip in minutes (ex. 350 = 5:50): ";
    std::cin >> startTime;

    std::cout << std::endl;
    std::cout << "Calculating your routes by all 3 priorities...\n\n";

    // Applying the algorithms.
    unsigned shortestTime = transportGrid.findShortestTimeBetweenBusStops(startBusStopName, endBusStopName, startTime, shortestTimePath);
    printShortestTimePath(shortestTime, startTime, startBusStopName, endBusStopName, shortestTimePath);
    std::cout << std::endl;
    unsigned minimalBusesTakenCount = transportGrid.findMinBusesTakenPath(startBusStopName, endBusStopName, minimalBusesTakenPath);
    printMinimalBusesTakenPath(minimalBusesTakenCount, minimalBusesTakenPath);
    std::cout << std::endl;
    unsigned minimalTimeSpentOutsideVehicle = transportGrid.findMinTimeSpentOutsideVehiclePath(startTime, startBusStopName, endBusStopName, minimumTimeSpentPath);
    printMinimumTimeSpentPath(minimalTimeSpentOutsideVehicle, minimumTimeSpentPath);

    // Serialize the transport grid if changes are made.
    transportGrid.serialize();

    std::cout << std::endl;


}
