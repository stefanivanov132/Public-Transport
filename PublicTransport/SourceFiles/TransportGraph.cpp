#include "../Headers/TransportGraph.h"

void TransportGraph::validateFiles(std::ifstream& busesWithStops, std::ifstream& timeTable)
{
	if (!busesWithStops.is_open())
	{
		throw std::runtime_error("Error: Cannot open file busesWithStops.");
	}

	if (!timeTable.is_open())
	{
		throw std::runtime_error("Error: Cannot open file timeTable.");
	}

	std::string lineFromBusesWithStops;
	if (!std::getline(busesWithStops, lineFromBusesWithStops) || lineFromBusesWithStops.empty())
	{
		throw std::runtime_error("Error: File busesWithStops is empty or invalid.");
	}

	std::string lineFromTimeTable;
	if (!std::getline(timeTable, lineFromTimeTable) || lineFromTimeTable.empty())
	{
		throw std::runtime_error("Error: File timeTable is empty or invalid.");
	}

	busesWithStops.clear();
	busesWithStops.seekg(0, std::ios::beg);

	timeTable.clear();
	timeTable.seekg(0, std::ios::beg);

}

void TransportGraph::deserializeBusesWithStops(std::ifstream& busesWithStops)
{
	std::string busWithStops;
	std::string busNumber;
	std::string currBusStopName;
	std::vector<std::string> currentRoute;

	while (!busesWithStops.eof())
	{
		std::getline(busesWithStops, busWithStops);
		std::stringstream strStream(busWithStops);
		std::getline(strStream, busNumber, ',');
		while (std::getline(strStream, currBusStopName, ','))
		{
			currentRoute.push_back(currBusStopName);
		}
		busToRoute.insert({ std::stoi(busNumber), currentRoute });
		currentRoute.clear();
	}
}

void TransportGraph::deserializeTimeTable(std::ifstream& timeTable)
{
	std::string timeTableString;
	std::string busStopName;
	std::string nextBusStopName;
	std::string busNumber;
	std::string currBusTimeForArrival;
	std::vector<unsigned> currentSchedule;

	while (!timeTable.eof())
	{
		std::getline(timeTable, timeTableString);
		std::stringstream strStream(timeTableString);
		std::getline(strStream, busStopName, ',');

		if (nameToBusStop.find(busStopName) != nameToBusStop.end())
		{
			const auto& currBusStop = nameToBusStop.find(busStopName);
			insertBusWithSchedule(strStream, busNumber, currentSchedule);
			currBusStop->second.setBusNumberToSchedule(std::stoi(busNumber), currentSchedule);
			currentSchedule.clear();
			continue;
		}

		BusStop currStop(busStopName);
		insertBusWithSchedule(strStream, busNumber, currentSchedule);
		currStop.setBusNumberToSchedule(std::stoi(busNumber), currentSchedule);
		currentSchedule.clear();

		nameToBusStop.insert({ busStopName, currStop });
	}
}

void TransportGraph::insertBusWithSchedule(std::stringstream& strStream, std::string& busNumber, std::vector<unsigned>& currentSchedule)
{
	std::string currBusTimeForArrival;
	std::getline(strStream, busNumber, ',');
	while (std::getline(strStream, currBusTimeForArrival, ','))
	{
		currentSchedule.push_back(std::stoi(currBusTimeForArrival));
	}
}

TransportGraph::TransportGraph(const std::string& fileBusesWithStops, const std::string& fileTimeTable)
{
	deserialize(fileBusesWithStops, fileTimeTable);
}

void TransportGraph::deserialize(const std::string& fileBusesWithStops, const std::string& fileTimeTable)
{
	std::ifstream busesWithStops(fileBusesWithStops);
	std::ifstream timeTable(fileTimeTable);

	try
	{
		validateFiles(busesWithStops, timeTable);
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error("Deserialization failed: " + std::string(e.what()));
	}
	deserializeBusesWithStops(busesWithStops);
	deserializeTimeTable(timeTable);
	fillTransportGrid();
}


unsigned TransportGraph::shortestTimeBetweenTwoBusStops(std::string& startBusStop, std::string& endBusStop, unsigned startTime, std::vector<vertexAndDistancePair>& path)
{
	return DijkstraAlgorithmForPublicTransport(
		[this](unsigned startTime, unsigned, unsigned currTime, unsigned currBus, unsigned nextBus, const std::string& currStop, const std::string& nextStop) 
		{
			return this->dynamicEdgeWeightShortestTime(startTime, currTime, currBus, nextBus, currStop, nextStop);
		},
		startBusStop, endBusStop, startTime, path
	);
}

unsigned TransportGraph::minimalBusesTaken(std::string& startBusStop, std::string& endBusStop, std::vector<vertexAndDistancePair>& path)
{
	std::pair<bool, unsigned> foundPathWithBusNumber = checkDirectPath(startBusStop, endBusStop);
	if (foundPathWithBusNumber.first)
	{
		path.push_back({ startBusStop, 0, 0, 0 });
		path.push_back({ endBusStop, foundPathWithBusNumber.second, 0, 0 });
		return 1;
	}
	return DijkstraAlgorithmForPublicTransport(
		[this](unsigned, unsigned, unsigned, unsigned currBus, unsigned nextBus, const std::string&, const std::string&) 
		{
			return this->dynamicEdgeWeightMinimalBusesTaken(currBus, nextBus);
		},
		startBusStop, endBusStop, 0, path
	);
}

unsigned TransportGraph::minimumTimeSpentOutsideVehicle(unsigned startTime, std::string& startBusStop, std::string& endBusStop, std::vector<vertexAndDistancePair>& path)
{
	return DijkstraAlgorithmForPublicTransport(
		[this, startTime](unsigned, unsigned currTime, unsigned, unsigned currBus, unsigned nextBus, const std::string& currStop, const std::string& nextStop) 
		{
			return this->dynamicEdgeWeightMinimumTimeSpent(currTime, currBus, nextBus, currStop, nextStop);
		},
		startBusStop, endBusStop, startTime, path
	);
}

unsigned TransportGraph::DijkstraAlgorithmForPublicTransport(
	std::function<std::pair<unsigned, unsigned>(unsigned, unsigned, unsigned, unsigned, unsigned, const std::string&, const std::string&)> weightUpdatingFunc,
	std::string& startBusStop, std::string& endBusStop, unsigned startTime, std::vector<vertexAndDistancePair>& path)
{
	
	std::unordered_map<std::string, unsigned> distances;
	for (const auto& pair : publicTransportGrid)
	{
		distances.insert({ pair.first.getBusStopName(), INT_MAX });
	}
	std::unordered_map<vertexAndDistancePair, vertexAndDistancePair> previous;

	std::priority_queue<vertexAndDistancePair> q;
	distances[startBusStop] = 0;
	vertexAndDistancePair start = { startBusStop, 0, 0, startTime};

	q.push(start);
	while (!q.empty())
	{
		vertexAndDistancePair current = q.top();
		q.pop();

		if (current.vertex == endBusStop)
		{
			unsigned goalResult = distances[current.vertex];
			while (!(current == start))
			{
				path.push_back(current);
				current = previous[current];
			}
			path.push_back(start);

			std::reverse(path.begin(), path.end());
			return goalResult;
		}

		for (auto& edge : publicTransportGrid.find(current.vertex)->second)
		{
			std::string currNeighbor = edge.busStop.getBusStopName();
			std::pair<unsigned, unsigned> dynamicWeightPair = weightUpdatingFunc(
				startTime, current.currentTime, current.distFromStart, current.busTakenToVertex, edge.busNumber, current.vertex, currNeighbor
			);
			
			unsigned dynamicWeight = dynamicWeightPair.first;

			if (distances[current.vertex] + dynamicWeight < distances[currNeighbor])
			{
				distances[currNeighbor] = distances[current.vertex] + dynamicWeight;
				vertexAndDistancePair next = { currNeighbor, edge.busNumber, distances[currNeighbor], dynamicWeightPair.second };	
				q.push(next);
				previous[next] = current;
			}
		}

	}
	return INT_MAX;
}


void TransportGraph::addBusAndRoute(const std::vector<std::string>& newRoute, unsigned busNumber)
{
	if (busToRoute.find(busNumber) == busToRoute.end())
	{
		busToRoute.insert({ busNumber, newRoute });
		return;
	}
	busToRoute[busNumber] = newRoute;
	return;
}

void TransportGraph::removeBusAndRoute(unsigned busNumber)
{
	if (busToRoute.find(busNumber) == busToRoute.end())
		return;
	busToRoute.erase(busNumber);
}

void TransportGraph::removeBusStop(const std::string& busStopName)
{
	if (nameToBusStop.find(busStopName) == nameToBusStop.end())
		return;
	nameToBusStop.erase(busStopName);
}

void TransportGraph::addNewBusStop(const std::string& newBusStopName, const std::unordered_map<unsigned, std::vector<unsigned>>& newBusStopBusToSchedule)
{
	BusStop newBusStop(newBusStopName);
	newBusStop.setBusToSchedule(newBusStopBusToSchedule);
}

void TransportGraph::showPassingBusesForBusStop(const std::string& busStopName)
{
	if (nameToBusStop.find(busStopName) == nameToBusStop.end())
	{
		std::cerr << "Bus Stop with such name doesn't exist!";
		return;
	}
	std::cout << "Passing buses through " + busStopName + ":\n";
	for (const auto& pair : nameToBusStop.find(busStopName)->second.getBusToSchedule())
	{
		std::cout << pair.first << " ";
	}
	return;
}

void TransportGraph::showScheduleForBus(const std::string& busStopName, unsigned busNumber)
{
	if (nameToBusStop.find(busStopName) == nameToBusStop.end())
	{
		std::cerr << "Bus Stop with such name doesn't exist!";
		return;
	}
	const auto& currBusStop = nameToBusStop.find(busStopName)->second;
	if (currBusStop.getBusToSchedule().find(busNumber) == currBusStop.getBusToSchedule().end())
	{
		std::cerr << "Bus with such number doesn't exist!";
		return;
	}
	const auto& busToSchedule = currBusStop.getBusToSchedule();
	std::cout << "Schedule for bus " + std::to_string(busNumber) + ":\n";
	for (const auto time : busToSchedule.at(busNumber))
	{
		std::cout << time << ", ";
	}
	return;
}

void TransportGraph::serialize()
{
	const std::string busesWithStopsName = "BusesWithStops.csv";
	const std::string timeTableName = "TimeTable.csv";
	try 
	{
		clearFile(busesWithStopsName);
		clearFile(timeTableName);

		std::ofstream busesWithStops(busesWithStopsName);
		std::ofstream timeTable(timeTableName);
		if (!busesWithStops.is_open()) 
		{
			throw std::runtime_error("Error: Unable to open file " + busesWithStopsName + " for writing.");
		}
		if (!timeTable.is_open())
		{
			throw std::runtime_error("Error: Unable to open file " + timeTableName + " for writing.");
		}

		serializeBusesWithStops(busesWithStops);
		serializeTimeTable(timeTable);
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Serialization failed: " << e.what() << std::endl;
	}
}


void TransportGraph::fillTransportGrid()
{
	if (!publicTransportGrid.empty())
	{
		clear();
	}
	for (auto i = nameToBusStop.begin(); i != nameToBusStop.end(); i++)
	{
		publicTransportGrid.insert({ i->second, {} });
	}

	for (auto it = busToRoute.begin(); it != busToRoute.end(); it++)
	{
		unsigned currBusNumber = it->first;
		std::vector<std::string> currBusRoute = it->second;
		auto currBusStopOfCurrBus = publicTransportGrid.find(currBusRoute[0]);
		for (size_t i = 1; i < currBusRoute.size(); i++)
		{
			auto nextBusStop = publicTransportGrid.find(currBusRoute[i]);
			currBusStopOfCurrBus->second.push_back({ nextBusStop->first, currBusNumber, 0 });
			currBusStopOfCurrBus = nextBusStop;
		}
	}
}

void TransportGraph::clear()
{
	publicTransportGrid.clear();
}

void TransportGraph::clearFile(const std::string& filename)
{
	std::ofstream file(filename, std::ios::trunc);
	if (!file.is_open()) 
	{
		throw std::runtime_error("Error: Unable to open file " + filename + " for clearing.");
	}
}

void TransportGraph::serializeBusesWithStops(std::ofstream& busesWithStops)
{
	unsigned mapSize = busToRoute.size();
	for (const auto& pair : busToRoute)
	{
		mapSize--;
		busesWithStops << pair.first << ",";
		for (size_t i = 0; i < pair.second.size()-1; i++)
		{
			busesWithStops << pair.second[i] << ",";
		}
		if (mapSize == 0)
		{
			busesWithStops << pair.second[pair.second.size() - 1];
		}
		else
		{
			busesWithStops << pair.second[pair.second.size() - 1] << "\n";
		}
	}

	busesWithStops.close();
}

void TransportGraph::serializeTimeTable(std::ofstream& timeTable)
{
	unsigned nameToBusSize = nameToBusStop.size();
	for (const auto& pair : nameToBusStop)
	{
		nameToBusSize--;
		unsigned mapSize = pair.second.getBusToSchedule().size();
		for (const auto& busToSchedulePair : pair.second.getBusToSchedule())
		{
			mapSize--;
			timeTable << pair.first << "," << busToSchedulePair.first << ",";
			for (size_t i = 0; i < busToSchedulePair.second.size() - 1; i++)
			{
				timeTable << busToSchedulePair.second[i] << ",";
			}
			if (mapSize == 0)
			{
				timeTable << busToSchedulePair.second[busToSchedulePair.second.size() - 1];
			}
			else
			{
				timeTable << busToSchedulePair.second[busToSchedulePair.second.size() - 1] << "\n";
			}
		}
		if (nameToBusSize != 0)
		{
			timeTable << "\n";
		}
	}

	timeTable.close();
}

std::pair<bool, unsigned> TransportGraph::checkDirectPath(const std::string& startBusStopName, const std::string& endBusStopName)
{
	for (const auto& pair : busToRoute)
	{
		for (size_t i = 0; i < pair.second.size(); i++)
		{
			if (pair.second[i] == startBusStopName)
			{
				for (size_t j = i + 1; j < pair.second.size(); j++)
				{
					if (pair.second[j] == endBusStopName)
					{
						return { true, pair.first };
					}
				}
			}
		}
	}
	return { false, 0 };
}

std::pair<unsigned, unsigned> TransportGraph::dynamicEdgeWeightShortestTime(unsigned startTime, unsigned currTimeFromStart, unsigned currBusTakenNumber, unsigned nextBusNumber, const std::string& currBusStopName, const std::string& nextBusStopName)
{
	const auto& startBusStop = publicTransportGrid.find(currBusStopName);
	const auto& nextBusStop = publicTransportGrid.find(nextBusStopName);

	unsigned currTime = startTime + currTimeFromStart;
	unsigned travelTime;

	if (currBusTakenNumber == nextBusNumber)
	{
		std::vector<unsigned> startBusStopSchedule = startBusStop->first.getBusSchedule(currBusTakenNumber);
		for (size_t i = 0; i < startBusStopSchedule.size(); i++)
		{
			if (startBusStopSchedule[i] == currTime)
			{
				travelTime = nextBusStop->first.getBusSchedule(nextBusNumber)[i] - currTime;
				return { travelTime, startBusStopSchedule[i] };
			}
		}
	}
	
	std::vector<unsigned> startBusStopSchedule = startBusStop->first.getBusSchedule(nextBusNumber);
	for (size_t i = 0; i < startBusStopSchedule.size(); i++)
	{
		if (startBusStopSchedule[i] >= currTime)
		{
			travelTime = (startBusStopSchedule[i] - currTime) + (nextBusStop->first.getBusSchedule(nextBusNumber)[i] - startBusStopSchedule[i]);
			return { travelTime, startBusStopSchedule[i] };
		}
	}

}

std::pair<unsigned,unsigned> TransportGraph::dynamicEdgeWeightMinimalBusesTaken(unsigned currBusTakenNumber, unsigned nextBusNumber)
{
	if (currBusTakenNumber == nextBusNumber)
	{
		return { 0, 0 };
	}
	else
	{
		return { 1, 0 };
	}
}

std::pair<unsigned, unsigned> TransportGraph::dynamicEdgeWeightMinimumTimeSpent(unsigned currTime, unsigned currBusTakenNumber, unsigned nextBusNumber, const std::string& currBusStopName, const std::string& nextBusStopName)
{
	const auto& startBusStop = publicTransportGrid.find(currBusStopName);
	const auto& nextBusStop = publicTransportGrid.find(nextBusStopName);
	if (currBusTakenNumber == nextBusNumber)
	{
		std::vector<unsigned> startBusStopSchedule = startBusStop->first.getBusSchedule(nextBusNumber);
		for (size_t i = 0; i < startBusStopSchedule.size(); i++)
		{
			if (startBusStopSchedule[i] == currTime)
			{
				currTime = nextBusStop->first.getBusSchedule(nextBusNumber)[i];
				return { 0, currTime };
			}
		}
	}
	unsigned edgeWeight;
	std::vector<unsigned> startBusStopSchedule = startBusStop->first.getBusSchedule(nextBusNumber);
	for (size_t i = 0; i < startBusStopSchedule.size(); i++)
	{
		if (startBusStopSchedule[i] >= currTime)
		{
			edgeWeight = startBusStopSchedule[i] - currTime;
			currTime = nextBusStop->first.getBusSchedule(nextBusNumber)[i];
			return {edgeWeight, currTime};
		}
	}
}

const std::unordered_map<unsigned, std::vector<std::string>>& TransportGraph::getRoutes() const
{
	return busToRoute;
}

const std::unordered_map<std::string, BusStop>& TransportGraph::getBusStopWithSchedule() const
{
	return nameToBusStop;
}

const std::unordered_map<BusStop, std::vector<TransportEdge>>& TransportGraph::getGrid() const
{
	return publicTransportGrid;
}
