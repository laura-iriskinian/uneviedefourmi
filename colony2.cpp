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
map<string, string> roomOccupied;

const int NUM_ANTS = 5;

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
    // === Fourmilière 2 avec un cycle ===
    addTunnel("Sv", "S1");
    addTunnel("S1", "S2");
    addTunnel("S2", "Sd");
    addTunnel("Sd", "Sv"); // Creates a possible shortcut

    // Create ants
    vector<Ant> ants;
    for (int i = 1; i <= NUM_ANTS; ++i) {
        ants.push_back(Ant("f" + to_string(i)));
    }

    // Only S1 and S2 can be occupied by one ant
    roomOccupied["S1"] = "";
    roomOccupied["S2"] = "";

    int step = 1;
    bool allFinished = false;

    while (!allFinished) {
        allFinished = true;
        bool anyMove = false;

        vector<pair<int, string>> plannedMoves;
        set<string> reservedRooms;
        set<string> releasedRooms;

        // Plan moves based on shortest paths
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
                bool willBeFree = (roomOccupied[next] == "" || releasedRooms.count(next));
                bool notReserved = reservedRooms.count(next) == 0;

                if (willBeFree && notReserved) {
                    canMove = true;
                }
            }

            if (canMove) {
                plannedMoves.push_back({i, next});
                if (next != "Sd") reservedRooms.insert(next);
                if (current != "Sv" && current != "Sd") releasedRooms.insert(current);
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

                if (from != "Sv" && from != "Sd") roomOccupied[from] = "";
                if (to != "Sv" && to != "Sd") roomOccupied[to] = ants[idx].name;

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
