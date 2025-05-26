#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <string>

using namespace std;

// Ant class
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

const int NUM_ANTS = 50;

// Add bidirectional tunnel
void addTunnel(const string& a, const string& b) {
    colony[a].push_back(b);
    colony[b].push_back(a);
}

// Find all shortest paths from start to target
vector<vector<string>> findAllShortestPaths(string start, string target) {
    queue<vector<string>> q;
    map<string, int> distances;
    vector<vector<string>> allPaths;

    q.push({start});
    distances[start] = 0;
    int minLength = INT_MAX;

    while (!q.empty()) {
        vector<string> path = q.front();
        q.pop();
        string last = path.back();
        int currentLength = path.size() - 1;

        // Skip if we already found shorter paths
        if (currentLength > minLength) continue;

        if (last == target) {
            if (currentLength < minLength) {
                minLength = currentLength;
                allPaths.clear(); // New shortest path found
            }
            if (currentLength == minLength) {
                allPaths.push_back(path);
            }
            continue;
        }

        for (const string& neighbor : colony[last]) {
            if (distances.count(neighbor) == 0 || distances[neighbor] >= currentLength + 1) {
                distances[neighbor] = currentLength + 1;
                vector<string> newPath = path;
                newPath.push_back(neighbor);
                q.push(newPath);
            }
        }
    }

    return allPaths;
}

// Smart path selection: chooses intelligently among equivalent paths
vector<string> smartPath(string start, string target, int antIndex, const map<string, int>& currentTempOccupancy) {
    vector<vector<string>> allPaths = findAllShortestPaths(start, target);

    if (allPaths.empty()) return {};

    cout << "  [SMART] Ant f" << antIndex + 1 << " - Possible paths from " << start << " to " << target << ":" << endl;
    for (int i = 0; i < allPaths.size(); i++) {
        cout << "    [" << i << "] ";
        for (int j = 0; j < allPaths[i].size(); j++) {
            cout << allPaths[i][j];
            if (j < allPaths[i].size() - 1) cout << " -> ";
        }
        cout << endl;
    }

    // Selection strategy: prefer paths with less congested next room
    int bestPathIndex = 0;
    int minCongestion = INT_MAX;

    for (int i = 0; i < allPaths.size(); i++) {
        if (allPaths[i].size() < 2) continue; // Already at destination

        string nextRoom = allPaths[i][1];
        int congestion = 0;

        if (nextRoom != "Sd" && currentTempOccupancy.count(nextRoom)) {
            congestion = currentTempOccupancy.at(nextRoom);
        }

        cout << "    Path [" << i << "] next room " << nextRoom << " congestion: " << congestion << endl;

        if (congestion < minCongestion) {
            minCongestion = congestion;
            bestPathIndex = i;
        }
    }

    cout << "  [SMART] Ant f" << antIndex + 1 << " chooses path [" << bestPathIndex << "] (least congested)" << endl;

    return allPaths[bestPathIndex];
}

// DEBUG: Function to display room status
void printRoomStatus() {
    cout << "  [STATE] Room occupancy: ";
    for (auto& room : roomOccupancy) {
        if (room.second > 0) {
            cout << room.first << ":" << room.second << "/" << roomCapacity[room.first] << " ";
        }
    }
    cout << endl;
}

// DEBUG: Function to display ant positions
void printAntPositions(const vector<Ant>& ants) {
    cout << "  [ANTS] Positions: ";
    map<string, vector<string>> positionGroups;
    for (const auto& ant : ants) {
        if (!ant.finished) {
            positionGroups[ant.position].push_back(ant.name);
        }
    }

    for (auto& group : positionGroups) {
        cout << group.first << ":(";
        for (int i = 0; i < group.second.size(); i++) {
            cout << group.second[i];
            if (i < group.second.size() - 1) cout << ",";
        }
        cout << ") ";
    }
    cout << endl;
}

int main() {
    // Build the colony layout
    addTunnel("Sv", "S1");
    addTunnel("S1", "S2");
    addTunnel("S2", "S3");
    addTunnel("S3", "S4");
    addTunnel("S4", "Sd");
    addTunnel("S2", "S5");
    addTunnel("S5", "S4");
    addTunnel("S13", "Sd");
    addTunnel("S8", "S12");
    addTunnel("S12", "S13");
    addTunnel("S6", "S7");
    addTunnel("S7", "S9");
    addTunnel("S9", "S14");
    addTunnel("S14", "Sd");
    addTunnel("S7", "S10");
    addTunnel("S10", "S14");
    addTunnel("S1", "S6");
    addTunnel("S6", "S8");
    addTunnel("S8", "S11");
    addTunnel("S11", "S13");

    // Define max occupancy per room
    roomCapacity = {
        {"S1", 8},
        {"S2", 4},
        {"S3", 2},
        {"S4", 4},
        {"S5", 2},
        {"S6", 4},
        {"S7", 2},
        {"S8", 5},
        {"S9", 1},
        {"S10", 1},
        {"S11", 1},
        {"S12", 1},
        {"S13", 4},
        {"S14", 2},
    };

    // Initialize room occupancy to 0
    for (const auto& room : roomCapacity) {
        roomOccupancy[room.first] = 0;
    }

    // Create ants
    vector<Ant> ants;
    for (int i = 1; i <= NUM_ANTS; ++i) {
        ants.push_back(Ant("f" + to_string(i)));
    }

    cout << "\nColony Four \n" << endl;
    cout << "Number of ants: " << ants.size() << endl;
    cout << endl;

    // DEBUG: Display all possible paths from Sv
    cout << "[DEBUG] Analysis of possible paths from Sv:" << endl;
    vector<vector<string>> initialPaths = findAllShortestPaths("Sv", "Sd");
    for (int i = 0; i < initialPaths.size(); i++) {
        cout << "  Path " << i << ": ";
        for (int j = 0; j < initialPaths[i].size(); j++) {
            cout << initialPaths[i][j];
            if (j < initialPaths[i].size() - 1) cout << " -> ";
        }
        cout << endl;
    }
    cout << endl;

    int step = 1;
    bool allFinished = false;

    while (!allFinished) {
        allFinished = true;
        bool anyMove = false;

        cout << "[DEBUG] === PLANNING STEP " << step << " ===" << endl;
        printAntPositions(ants);
        printRoomStatus();

        vector<pair<int, string>> plannedMoves;
        map<string, int> tempOccupancy = roomOccupancy; // Copy of current occupancy

        // Sequential planning with immediate update of temporary occupancy
        for (int i = 0; i < ants.size(); ++i) {
            Ant& ant = ants[i];
            if (ant.finished) continue;

            string current = ant.position;
            cout << "  Planning for " << ant.name << " (at " << current << "):" << endl;

            vector<string> path = smartPath(current, "Sd", i, tempOccupancy);
            if (path.size() < 2) {
                cout << "    -> Already arrived" << endl;
                continue;
            }

            string next = path[1];
            cout << "    -> Wants to go to " << next << endl;

            bool canMove = false;

            if (next == "Sd") {
                canMove = true;
                cout << "    -> Can go to Sd (final destination)" << endl;
            } else {
                // Use temporary occupancy (includes already planned moves)
                int currentTempOccupancy = tempOccupancy[next];
                bool hasSpace = currentTempOccupancy < roomCapacity[next];

                cout << "    -> " << next << " temporary state: occupied=" << currentTempOccupancy
                     << ", capacity=" << roomCapacity[next] << endl;

                if (hasSpace) {
                    canMove = true;
                    cout << "    -> Movement authorized" << endl;
                } else {
                    cout << "    -> Movement blocked (room full)" << endl;
                }
            }

            if (canMove) {
                plannedMoves.push_back({i, next});

                // Immediately update temporary occupancy
                if (current != "Sv" && current != "Sd") {
                    tempOccupancy[current]--;
                }
                if (next != "Sv" && next != "Sd") {
                    tempOccupancy[next]++;
                }

                anyMove = true;
                cout << "    -> PLANNED MOVEMENT: " << ant.name << " to " << next << endl;
            }

            if (ant.position != "Sd") allFinished = false;
        }

        // Apply all planned movements
        if (anyMove) {
            cout << "\n+++ Step " << step << " +++" << endl;

            for (auto& move : plannedMoves) {
                int idx = move.first;
                string from = ants[idx].position;
                string to = move.second;

                // Update real occupancy
                if (from != "Sv" && from != "Sd") roomOccupancy[from]--;
                if (to != "Sv" && to != "Sd") roomOccupancy[to]++;

                ants[idx].position = to;
                if (to == "Sd") ants[idx].finished = true;

                cout << ants[idx].name << " moves to " << to << endl;
            }

            cout << endl;
            step++;
        } else {
            cout << "[DEBUG] No movement possible - STOP" << endl;
            break;
        }
    }

    cout << "All ants have reached Sd in " << step - 1 << " steps!" << endl;
    return 0;
}