#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <set>
#include <algorithm>
#include <limits>

using namespace std;

// Ant representation
class Ant {
public:
    string name;
    string position;
    bool finished;

    Ant(string n) : name(n), position("Sv"), finished(false) {}
};

map<string, vector<string>> colony;              // Graph: room -> connected rooms
map<string, int> roomCapacity;                   // Room name -> capacity
map<string, vector<string>> roomOccupants;       // Room name -> list of ant names
vector<Ant> ants;

// Read configuration from input.txt
bool readInput(const string& filename, int& numAnts) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Cannot open " << filename << endl;
        return false;
    }

    int numRooms, numTunnels;
    file >> numAnts;

    file >> numRooms;
    for (int i = 0; i < numRooms; ++i) {
        string name;
        int cap;
        file >> name >> cap;
        if (cap == -1) cap = numeric_limits<int>::max(); // Infinite capacity
        roomCapacity[name] = cap;
        roomOccupants[name] = {};
    }

    file >> numTunnels;
    for (int i = 0; i < numTunnels; ++i) {
        string a, b;
        file >> a >> b;
        colony[a].push_back(b);
        colony[b].push_back(a);

        // Ensure rooms exist in the maps even if not declared earlier
        if (!roomCapacity.count(a)) {
            roomCapacity[a] = 1;
            roomOccupants[a] = {};
        }
        if (!roomCapacity.count(b)) {
            roomCapacity[b] = 1;
            roomOccupants[b] = {};
        }
    }

    file.close();
    return true;
}

// Breadth-first search to find the shortest path from start to target
vector<string> shortestPath(const string& start, const string& target) {
    queue<vector<string>> q;
    set<string> visited;
    q.push({start});
    visited.insert(start);

    while (!q.empty()) {
        vector<string> path = q.front();
        q.pop();
        string last = path.back();
        if (last == target) return path;

        for (const string& neighbor : colony[last]) {
            if (!visited.count(neighbor)) {
                visited.insert(neighbor);
                vector<string> newPath = path;
                newPath.push_back(neighbor);
                q.push(newPath);
            }
        }
    }
    return {};
}

// Verify that Sd is reachable from Sv
bool isReachable() {
    if (!roomCapacity.count("Sv") || !roomCapacity.count("Sd")) {
        cerr << "[ERROR] Room Sv or Sd is missing." << endl;
        return false;
    }

    vector<string> path = shortestPath("Sv", "Sd");
    if (path.empty()) return false;

    // ✅ Show the path for debugging
    cout << "[INFO] Path from Sv to Sd: ";
    for (const string& r : path) cout << r << " ";
    cout << endl;
    return true;
}

// Check if room can be entered during this step
bool canEnter(const string& room, const set<string>& willBeFreed, const map<string, vector<string>>& tempOccupants) {
    if (room == "Sv" || room == "Sd") return true;
    int current = tempOccupants.at(room).size();
    int capacity = roomCapacity[room];
    return current < capacity || willBeFreed.count(room) > 0;
}

int main() {
    int numAnts;
    if (!readInput("input.txt", numAnts)) return 1;

    // Create ants and place them in Sv
    for (int i = 1; i <= numAnts; ++i) {
        ants.push_back(Ant("f" + to_string(i)));
        roomOccupants["Sv"].push_back(ants.back().name);
    }

    // Validate graph
    if (!isReachable()) {
        cerr << "[ERROR] Sd is not reachable from Sv. Check your input.txt." << endl;
        return 1;
    }

    int step = 1;
    bool allFinished = false;

    while (!allFinished) {
        allFinished = true;
        bool anyMove = false;

        map<string, vector<string>> tempOccupants = roomOccupants;
        set<string> releasedRooms;
        vector<pair<int, string>> plannedMoves;

        // Plan movements
        for (int i = 0; i < ants.size(); ++i) {
            Ant& ant = ants[i];
            if (ant.finished) continue;

            string current = ant.position;
            vector<string> path = shortestPath(current, "Sd");
            if (path.size() < 2) continue;

            string next = path[1];

            if (canEnter(next, releasedRooms, tempOccupants)) {
                plannedMoves.push_back({i, next});
                if (current != "Sv" && current != "Sd") releasedRooms.insert(current);
                tempOccupants[next].push_back(ant.name);
                tempOccupants[current].erase(remove(tempOccupants[current].begin(),
                    tempOccupants[current].end(), ant.name), tempOccupants[current].end());
                anyMove = true;
            }

            if (ant.position != "Sd") allFinished = false;
        }

        // Execute planned moves
        if (anyMove) {
            cout << "+++ Step " << step << " +++" << endl;
            for (auto& move : plannedMoves) {
                int idx = move.first;
                string from = ants[idx].position;
                string to = move.second;

                ants[idx].position = to;
                if (to == "Sd") ants[idx].finished = true;

                roomOccupants[to].push_back(ants[idx].name);
                if (from != "Sv" && from != "Sd") {
                    roomOccupants[from].erase(remove(roomOccupants[from].begin(),
                        roomOccupants[from].end(), ants[idx].name), roomOccupants[from].end());
                }

                cout << ants[idx].name << " moves to " << to << endl;
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
