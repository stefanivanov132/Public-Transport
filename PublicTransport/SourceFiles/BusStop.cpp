#include "BusStop.h"

BusStop::BusStop(const std::string& busStopName) : busStopName(busStopName) {}

bool BusStop::operator==(const BusStop& other) const
{
	return busStopName == other.busStopName;
}

const std::string& BusStop::getBusStopName() const
{
	return busStopName;
}

const std::string& BusStop::getBusStopName()
{
	return busStopName;
}

const std::vector<unsigned>& BusStop::getBusSchedule(unsigned busNumber)
{
	return busToSchedule[busNumber];
}

const std::vector<unsigned>& BusStop::getBusSchedule(unsigned busNumber) const
{
	return busToSchedule.at(busNumber);
}

void BusStop::setBusNumberToSchedule(unsigned busNumber, const std::vector<unsigned>& busSchedule)
{
	busToSchedule.insert({ busNumber, busSchedule });
}

void BusStop::setBusToSchedule(const std::unordered_map<unsigned, std::vector<unsigned>>& otherBusToSchedule)
{
	busToSchedule.clear();
	for (const auto& pair : otherBusToSchedule)
	{
		busToSchedule.insert({ pair.first, pair.second });
	}
	return;
}

const std::unordered_map<unsigned, std::vector<unsigned>>& BusStop::getBusToSchedule()
{
	return busToSchedule;
}

const std::unordered_map<unsigned, std::vector<unsigned>>& BusStop::getBusToSchedule() const
{
	return busToSchedule;
}