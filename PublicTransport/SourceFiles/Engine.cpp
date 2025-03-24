#include "Engine.h"

void Engine::run()
{
    // Names of the needed files to run the project.
	const std::string busesWithStops = "BusesWithStops.csv";
	const std::string timeTable = "TimeTable.csv";

    // Creating the transport grid.
	TransportGraph transportGrid(busesWithStops, timeTable);

    // Information needed from the user.
    std::string startBusStopName;
    std::string endBusStopName;
    unsigned startTime;
    std::vector<vertexAndDistancePair> shortestTimePath;
    std::vector<vertexAndDistancePair> minimalBusesTakenPath;
    std::vector<vertexAndDistancePair> minimumTimeSpentPath;

    std::cout << "Enter the name of the bus stop you are currently at: ";
    std::getline(std::cin, startBusStopName);

    std::cout << "Enter the name of the bus stop you want to reach: ";
    std::getline(std::cin, endBusStopName);

    std::cout << "Enter the starting time of your trip: ";
    std::cin >> startTime;

    std::cout << std::endl;
    std::cout << "Calculating your routes by all 3 priorities...\n\n";

    // Applying the algorithms.
    unsigned shortestTime = transportGrid.shortestTimeBetweenTwoBusStops(startBusStopName, endBusStopName, startTime, shortestTimePath);
    printShortestTimePath(shortestTime, startTime, startBusStopName, endBusStopName, shortestTimePath);
    std::cout << std::endl;
    unsigned minimalBusesTakenCount = transportGrid.minimalBusesTaken(startBusStopName, endBusStopName, minimalBusesTakenPath);
    printMinimalBusesTakenPath(minimalBusesTakenCount, minimalBusesTakenPath);
    std::cout << std::endl;
    unsigned minimalTimeSpentOutsideVehicle = transportGrid.minimumTimeSpentOutsideVehicle(startTime, startBusStopName, endBusStopName, minimumTimeSpentPath);
    printMinimumTimeSpentPath(minimalTimeSpentOutsideVehicle, minimumTimeSpentPath);

    // Serialize the transport grid if changes are made.
    transportGrid.serialize();

    std::cout << std::endl;


}
