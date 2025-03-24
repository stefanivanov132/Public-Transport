#pragma once
#include "BusStop.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <iostream>
#include <unordered_set>
#include <functional>

struct TransportEdge
{
	const BusStop& busStop;
	unsigned busNumber;
	unsigned edgeWeight;
	TransportEdge(const BusStop& busStop, unsigned busNumber, unsigned edgeWeight)
		: busStop(busStop), busNumber(busNumber), edgeWeight(edgeWeight) {}
};

struct vertexAndDistancePair
{
	std::string vertex;
	unsigned busTakenToVertex;
	unsigned distFromStart;
	unsigned currentTime;
	bool operator<(const vertexAndDistancePair& other) const
	{
		return distFromStart > other.distFromStart;
	}
	bool operator==(const vertexAndDistancePair& other) const
	{
		return vertex == other.vertex;
	}
};

namespace std {
	template <>
	struct hash<vertexAndDistancePair> {
		std::size_t operator()(const vertexAndDistancePair& current) const {
			return std::hash<std::string>()(current.vertex);
		}
	};
}

class TransportGraph
{
	std::unordered_map<unsigned, std::vector<std::string>> busToRoute;
	std::unordered_map<std::string, BusStop> nameToBusStop;
	std::unordered_map<BusStop, std::vector<TransportEdge>> publicTransportGrid;

	void validateFiles(std::ifstream& file1, std::ifstream& file2);
	void deserializeBusesWithStops(std::ifstream& busesWithStops);
	void deserializeTimeTable(std::ifstream& timeTable);
	void insertBusWithSchedule(std::stringstream& strStream, std::string& busNumber, std::vector<unsigned>& currentSchedule);
	void fillTransportGrid();
	void clear();
	void clearFile(const std::string& filename);
	void serializeBusesWithStops(std::ofstream& busesWithStops);
	void serializeTimeTable(std::ofstream& timeTable);

	std::pair<bool, unsigned> checkDirectPath(const std::string& startBusStopName, const std::string& endBusStopName);
	std::pair<unsigned, unsigned> dynamicEdgeWeightShortestTime(unsigned startTime, unsigned currTime, unsigned currBusTakenNumber, unsigned nextBusNumber, const std::string& currBusStopName, const std::string& nextBusStopName);
	std::pair<unsigned, unsigned> dynamicEdgeWeightMinimalBusesTaken(unsigned currBusTakenNumber, unsigned nextBusNumber);
	std::pair<unsigned, unsigned> dynamicEdgeWeightMinimumTimeSpent(unsigned currTime, unsigned currBusTakenNumber, unsigned nextBusNumber, const std::string& currBusStopName, const std::string& nextBusStopName);
	unsigned DijkstraAlgorithmForPublicTransport(
		std::function<std::pair<unsigned, unsigned>(unsigned, unsigned, unsigned, unsigned, unsigned, const std::string&, const std::string&)> weightUpdatingFunc,
		std::string& startBusStop, std::string& endBusStop, unsigned startTime, std::vector<vertexAndDistancePair>& path);
public:
	TransportGraph() = default;
	TransportGraph(const std::string& fileBusesWithStops, const std::string& fileTimeTable);
	void deserialize(const std::string& fileBusesWithStops, const std::string& fileTimeTable);
	unsigned shortestTimeBetweenTwoBusStops(std::string& startBusStop, std::string& endBusStop, unsigned startTime, std::vector<vertexAndDistancePair>& path);
	unsigned minimalBusesTaken(std::string& startBusStop, std::string& endBusStop, std::vector<vertexAndDistancePair>& path);
	unsigned minimumTimeSpentOutsideVehicle(unsigned startTime, std::string& startBusStop, std::string& endBusStop, std::vector<vertexAndDistancePair>& path);
	void addBusAndRoute(const std::vector<std::string>& newRoute, unsigned busNumber);
	void removeBusAndRoute(unsigned busNumber);
	void removeBusStop(const std::string& busStopName);
	void addNewBusStop(const std::string& newBusStopName, const std::unordered_map<unsigned, std::vector<unsigned>>& newBusStopBusToSchedule);
	void showPassingBusesForBusStop(const std::string& busStopName);
	void showScheduleForBus(const std::string& busStopName, unsigned busNumber);
	void serialize();
	const std::unordered_map<unsigned, std::vector<std::string>>& getRoutes() const;
	const std::unordered_map<std::string, BusStop>& getBusStopWithSchedule() const;
	const std::unordered_map<BusStop, std::vector<TransportEdge>>& getGrid() const;
};