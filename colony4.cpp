#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <set>

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

const int NUM_ANTS = 10;

// Add bidirectional tunnel
void addTunnel(const string& a, const string& b) {
    colony[a].push_back(b);
    colony[b].push_back(a);
}

// Find the shortest path from current to Sd using BFS
vector<string> shortestPath(string start, string target) {
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
    return {}; // No path
}

int main() {
    // Build the colony layout
    addTunnel("S3", "S4");
    addTunnel("Sv", "S1");
    addTunnel("S1", "S2");
    addTunnel("S2", "S4");
    addTunnel("S4", "S5");
    addTunnel("S5", "Sd");
    addTunnel("S4", "S6");
    addTunnel("S6", "Sd");
    addTunnel("S1", "S3"); // dead end

    // Create ants
    vector<Ant> ants;
    for (int i = 1; i <= NUM_ANTS; ++i) {
        ants.push_back(Ant("f" + to_string(i)));
    }

    // Define max occupancy per room
    map<string, int> roomCapacity = {
        {"S1", 2},  // S1 can hold 2 ants simultaneously
        {"S4", 2},  // S4 can hold 2 ants simultaneously
        {"S2", 1},  // Other rooms have a capacity of one ant max
        {"S3", 1},
        {"S5", 1},
        {"S6", 1}
    };

    // Initialize room occupancy to 0
    for (const auto& room : roomCapacity) {
        roomOccupancy[room.first] = 0;
    }

    int step = 1;
    bool allFinished = false;

    // Print the presentation lines
    cout << "\nColony four\n" << endl;
    cout << "Number of ants : "<< ants.size() << endl;

    while (!allFinished) {
        allFinished = true;
        bool anyMove = false;

        vector<pair<int, string>> plannedMoves;
        set<string> reservedRooms;
        set<string> releasedRooms;
        map<string, int> releasedAntsCount;

        // First pass: count how many ants will leave each room
        for (int i = 0; i < ants.size(); ++i) {
            Ant& ant = ants[i];
            if (ant.finished) continue;

            string current = ant.position;
            if (current != "Sv" && current != "Sd") {
                // Try to get path to Sd
                vector<string> path = shortestPath(current, "Sd");
                if (path.size() >= 2) {
                    // This ant might leave its current room
                    releasedAntsCount[current]++;
                }
            }
        }

        // Second pass, plan moves based on shortest paths
        for (int i = 0; i < ants.size(); ++i) {
            Ant& ant = ants[i];
            if (ant.finished) continue;

            string current = ant.position;

            vector<string> path = shortestPath(current, "Sd");
            if (path.size() < 2) continue; // Already at Sd

            string next = path[1];

            bool canMove = false;

            if (next == "Sd") {
                canMove = true;
            } else {
                // Check if there's space considering ants that will leave
                int currentOccupancy = roomOccupancy[next];
                int plannedOccupants = 0;
                for (auto& plannedMove : plannedMoves) {
                    if (ants[plannedMove.first].position != next && plannedMove.second == next) {
                        plannedOccupants++;
                    }
                }

                int potentialReleases = releasedAntsCount[next];
                bool hasSpace = (currentOccupancy - potentialReleases + plannedOccupants) < roomCapacity[next];

                if (hasSpace) {
                    canMove = true;
                }
            }

            if (canMove) {
                plannedMoves.push_back({i, next});
                anyMove = true;
            }

            if (ant.position != "Sd") allFinished = false;
        }

        // Apply planned moves
        if (anyMove) {
            cout << "+++ Step " << step << " +++" << endl;

            for (auto& move : plannedMoves) {
                int idx = move.first;
                string from = ants[idx].position;
                string to = move.second;

                if (from != "Sv" && from != "Sd") roomOccupancy[from]--;
                if (to != "Sv" && to != "Sd") roomOccupancy[to]++;

                ants[idx].position = to;
                if (to == "Sd") ants[idx].finished = true;

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
