# Public-Transport
DSA - Semester Project - FMI 2025

This C++ project simulates a public transport system where users can calculate optimal travel routes between bus stops based on different criteria:
- **Shortest Time**
- **Minimal Bus Transfers**
- **Minimum Time Spent Outside Vehicle**

## Features
- Graph representation of a public transport system
- Uses Dijkstra's algorithm with custom weight functions
- Load and save data from/to CSV files
- Semi-detailed route explanation for end-users

## Project Structure
```
├── BusStop.h / .cpp
├── Engine.h / .cpp
├── HelperFunctions.h / .cpp
├── TransportGraph.h / .cpp
├── StartUp.cpp
├── BusesWithStops.csv      # Required input file with bus routes
├── TimeTable.csv           # Required input file with bus schedules
```

## How It Works
1. **Deserialization**: Loads bus stop names, routes, and schedules from CSV files.
2. **User Input**: Prompts for start stop, end stop, and time.
3. **Route Planning**: Calculates best route based on all three optimization criteria.
4. **Output**: Displays step-by-step guidance for each criterion.
5. **Serialization**: Saves updated state back to CSV files if changes occur.

### Input Files
Ensure `BusesWithStops.csv` and `TimeTable.csv` are in the same directory.
