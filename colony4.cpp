#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>

using namespace std;

class Ant {
public:
    string name;
    string position;
    bool finished;

    Ant(string n) {
        name = n;
        position = "Sv";
        finished = false;
    }
};

// Graph structure
map<string, vector<string>> colony;
map<string, int> roomOccupancy;
map<string, int> roomCapacity;

const int NUM_ANTS = 10;

void addTunnel(const string& a, const string& b) {
    colony[a].push_back(b);
    colony[b].push_back(a);
}

// Simple BFS to find shortest path
vector<string> findShortestPath(string start, string target) {
    if (start == target) return {start};

    queue<vector<string>> q;
    map<string, bool> visited;

    q.push({start});
    visited[start] = true;

    while (!q.empty()) {
        vector<string> path = q.front();
        q.pop();
        string last = path.back();

        for (const string& neighbor : colony[last]) {
            if (neighbor == target) {
                path.push_back(neighbor);
                return path;
            }

            if (!visited[neighbor]) {
                visited[neighbor] = true;
                vector<string> newPath = path;
                newPath.push_back(neighbor);
                q.push(newPath);
            }
        }
    }

    return {}; // No path found
}

// Get all possible next moves for ants at given position
vector<string> getPossibleNextRooms(string currentPos) {
    vector<string> nextRooms;

    if (currentPos == "Sv") {
        nextRooms.push_back("S1"); // Only S1 from Sv
    }
    else if (currentPos == "S1") {
        nextRooms.push_back("S2");
        nextRooms.push_back("S3");
    }
    else if (currentPos == "S2") {
        nextRooms.push_back("S4");
    }
    else if (currentPos == "S3") {
        nextRooms.push_back("S4");
    }
    else if (currentPos == "S4") {
        nextRooms.push_back("S5");
        nextRooms.push_back("S6");
    }
    else if (currentPos == "S5") {
        nextRooms.push_back("Sd");
    }
    else if (currentPos == "S6") {
        nextRooms.push_back("Sd");
    }

    return nextRooms;
}

// Choose best next room based on distance to Sd and room availability
string chooseBestNextRoom(string currentPos, const map<string, int>& tempOccupancy) {
    vector<string> options = getPossibleNextRooms(currentPos);

    if (options.empty()) return "";

    // If only one option, return it
    if (options.size() == 1) {
        string nextRoom = options[0];
        if (nextRoom == "Sd") return nextRoom;

        int currentOccupancy = tempOccupancy.count(nextRoom) ? tempOccupancy.at(nextRoom) : 0;
        int capacity = roomCapacity.count(nextRoom) ? roomCapacity.at(nextRoom) : 1;

        if (currentOccupancy < capacity) {
            return nextRoom;
        }
        return ""; // Room full
    }

    // Multiple options - choose the one with shortest path to Sd and available space
    string bestRoom = "";
    int shortestDistance = INT_MAX;

    for (const string& room : options) {
        if (room == "Sd") return room; // Always prefer direct path to Sd

        int currentOccupancy = tempOccupancy.count(room) ? tempOccupancy.at(room) : 0;
        int capacity = roomCapacity.count(room) ? roomCapacity.at(room) : 1;

        if (currentOccupancy < capacity) {
            vector<string> pathToSd = findShortestPath(room, "Sd");
            if (!pathToSd.empty() && pathToSd.size() - 1 < shortestDistance) {
                shortestDistance = pathToSd.size() - 1;
                bestRoom = room;
            }
        }
    }

    return bestRoom;
}

void printRoomStatus() {
    cout << "  [STATE] Room occupancy: ";
    for (auto& room : roomOccupancy) {
        if (room.second > 0) {
            cout << room.first << ":" << room.second << "/" << roomCapacity[room.first] << " ";
        }
    }
    cout << endl;
}

void printAntPositions(const vector<Ant>& ants) {
    map<string, int> positionCount;
    for (const auto& ant : ants) {
        if (!ant.finished) {
            positionCount[ant.position]++;
        }
    }

    cout << "  [ANTS] Positions: ";
    for (auto& pos : positionCount) {
        cout << pos.first << ":" << pos.second << " ";
    }
    cout << endl;
}

int main() {
    // Build the colony layout for fourmiliere_quatre
    addTunnel("Sv", "S1");
    addTunnel("S1", "S2");
    addTunnel("S2", "S4");
    addTunnel("S3", "S4");
    addTunnel("S4", "S5");
    addTunnel("S5", "Sd");
    addTunnel("S4", "S6");
    addTunnel("S6", "Sd");
    addTunnel("S1", "S3");

    // Define room capacities
    roomCapacity = {
        {"S1", 2},
        {"S2", 1},
        {"S3", 1},
        {"S4", 2},
        {"S5", 1},
        {"S6", 1},
    };

    // Initialize room occupancy
    for (const auto& room : roomCapacity) {
        roomOccupancy[room.first] = 0;
    }

    // Create ants
    vector<Ant> ants;
    for (int i = 1; i <= NUM_ANTS; ++i) {
        ants.push_back(Ant("f" + to_string(i)));
    }

    cout << "\nColony Four\n" << endl;
    cout << "Number of ants: " << ants.size() << endl;
    cout << endl;

    int step = 1;
    bool allFinished = false;

    while (!allFinished && step <= 20) { // Safety limit
        allFinished = true;

        vector<pair<int, string>> plannedMoves;
        map<string, int> tempOccupancy = roomOccupancy;

        // Process ants in waves - prioritize by current position
        // Priority: closer to Sd gets priority to avoid blocking
        vector<int> antOrder;
        for (int i = 0; i < ants.size(); i++) {
            if (!ants[i].finished) {
                antOrder.push_back(i);
            }
        }

        // Sort ants: first by distance to Sd (closer first), then by ant number (f1, f2, f3...)
        sort(antOrder.begin(), antOrder.end(), [&](int a, int b) {
            vector<string> pathA = findShortestPath(ants[a].position, "Sd");
            vector<string> pathB = findShortestPath(ants[b].position, "Sd");

            // If same distance, sort by ant index (f1 before f2, etc.)
            if (pathA.size() == pathB.size()) {
                return a < b; // Lower index first (f1 before f2)
            }
            return pathA.size() < pathB.size(); // Closer ants first
        });

        for (int antIndex : antOrder) {
            Ant& ant = ants[antIndex];
            string current = ant.position;

            string nextRoom = chooseBestNextRoom(current, tempOccupancy);

            if (!nextRoom.empty()) {
                plannedMoves.push_back({antIndex, nextRoom});

                // Update temporary occupancy
                if (current != "Sv" && current != "Sd") {
                    tempOccupancy[current]--;
                }
                if (nextRoom != "Sv" && nextRoom != "Sd") {
                    tempOccupancy[nextRoom]++;
                }
            }

            if (ant.position != "Sd") allFinished = false;
        }

        // Apply planned moves
        if (!plannedMoves.empty()) {
            cout << "\n+++ step " << step << " +++" << endl;

            for (auto& move : plannedMoves) {
                int idx = move.first;
                string from = ants[idx].position;
                string to = move.second;

                // Update real occupancy
                if (from != "Sv" && from != "Sd") roomOccupancy[from]--;
                if (to != "Sv" && to != "Sd") roomOccupancy[to]++;

                ants[idx].position = to;
                if (to == "Sd") ants[idx].finished = true;

                cout << ants[idx].name << " - " << from << " - " << to << endl;
            }

            cout << endl;
            step++;
        } else {
            break;
        }
    }

    cout << "All ants have reached Sd in " << step - 1 << " steps!" << endl;
    return 0;
}