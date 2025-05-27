#ifndef ANTS_H
#define ANTS_H

#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <climits>

using namespace std;

// Class representing an ant
class Ant {
public:
    string name;
    string position;
    bool finished;

    Ant(string n);
};

// Structure to store ant colony information
struct ColonyInfo {
    int numAnts;
    map<string, int> roomCapacity;
    map<string, vector<string>> tunnels; // Connection graph
};

// Structure to store timing information
struct TimingInfo {
    clock_t start_total;
    clock_t start_load;
    clock_t end_load;
    clock_t start_simulation;
    clock_t end_simulation;
    clock_t end_total;

    TimingInfo() {
        start_total = clock();
        start_load = 0;
        end_load = 0;
        start_simulation = 0;
        end_simulation = 0;
        end_total = 0;
    }
};

// Global variables
extern ColonyInfo colonyInfo;
extern map<string, int> roomOccupancy;
extern TimingInfo timing;

// Functions for tunnel and colony management
void addTunnel(const string& a, const string& b);
bool loadColonyFromFile(const string& filename);

// Pathfinding and movement functions
vector<string> findShortestPath(string start, string target);
vector<string> getPossibleNextRooms(string currentPos);
string chooseBestNextRoom(string currentPos, const map<string, int>& tempOccupancy);

// Utility functions
bool compareRooms(const pair<string, int>& a, const pair<string, int>& b);
void printColonyInfo();

// Timing functions
void startLoadTimer();
void endLoadTimer();
void startSimulationTimer();
void endSimulationTimer();
void printTimingStatistics(int totalSteps);

#endif // ANTS_H