#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class BusStop
{
	std::string busStopName;
	std::unordered_map<unsigned, std::vector<unsigned>> busToSchedule;

public:
	BusStop(const std::string& busStopName);

	bool operator==(const BusStop& other) const;
	const std::string& getBusStopName() const;
	const std::string& getBusStopName();
	const std::vector<unsigned>& getBusSchedule(unsigned busNumber);
	const std::vector<unsigned>& getBusSchedule(unsigned busNumber) const;
	void setBusNumberToSchedule(unsigned busNumber, const std::vector<unsigned>& busSchedule);
	void setBusToSchedule(const std::unordered_map<unsigned, std::vector<unsigned>>& otherBusToSchedule);
	const std::unordered_map<unsigned, std::vector<unsigned>>& getBusToSchedule();
	const std::unordered_map<unsigned, std::vector<unsigned>>& getBusToSchedule() const;
};

namespace std {
	template <>
	struct hash<BusStop> {
		std::size_t operator()(const BusStop& busStop) const {
			return std::hash<std::string>()(busStop.getBusStopName());
		}
	};
}