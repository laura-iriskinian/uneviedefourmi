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

// Global variables
extern ColonyInfo colonyInfo;
extern map<string, int> roomOccupancy;

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

#endif // ANTS_H