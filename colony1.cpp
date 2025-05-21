#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <set>

using namespace std;

class Ant {
public:
    string name;
    string position;
    bool finished;

    Ant(string n) {
        name = n;
        position = "Sv"; // All ants start in the vestibule
        finished = false;
    }
};

// Graph of the colony
map<string, vector<string>> colony;

// Room occupancy: only needed for S1 and S2
map<string, string> roomOccupied;

const int NUM_ANTS = 5;

// Add undirected tunnel between rooms
void addTunnel(const string& a, const string& b) {
    colony[a].push_back(b);
    colony[b].push_back(a);
}

// Fixed path: Sv → S1 → S2 → Sd
string getNextRoom(const string& current) {
    if (current == "Sv") return "S1";
    if (current == "S1") return "S2";
    if (current == "S2") return "Sd";
    return "";
}

int main() {
    // Build the colony layout
    addTunnel("Sv", "S1");
    addTunnel("S1", "S2");
    addTunnel("S2", "Sd");

    // Create ants
    vector<Ant> ants;
    for (int i = 1; i <= NUM_ANTS; ++i) {
        ants.push_back(Ant("f" + to_string(i)));
    }

    // Initialize room occupancy
    roomOccupied["S1"] = "";
    roomOccupied["S2"] = "";

    int step = 1;
    bool allFinished = false;

    while (!allFinished) {
        allFinished = true;
        bool anyMove = false;

        vector<pair<int, string>> plannedMoves;
        set<string> reservedRooms;   // Rooms another ant plans to enter
        set<string> releasedRooms;   // Rooms that will be freed this step

        // First pass: plan all possible moves
        for (int i = 0; i < ants.size(); ++i) {
            Ant& ant = ants[i];
            if (ant.finished) continue;

            string current = ant.position;
            string next = getNextRoom(current);

            if (next.empty()) continue;

            bool canMove = false;

            if (next == "Sd") {
                canMove = true;
            } else {
                bool willBeFree = (roomOccupied[next] == "" || releasedRooms.count(next) > 0);
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

            if (ant.position != "Sd") {
                allFinished = false;
            }
        }

        // Only print and apply moves if there are any
        if (anyMove) {
            cout << "+++ Step " << step << " +++" << endl;

            for (auto& move : plannedMoves) {
                int idx = move.first;
                string from = ants[idx].position;
                string to = move.second;

                if (from != "Sv" && from != "Sd") {
                    roomOccupied[from] = "";
                }

                if (to != "Sv" && to != "Sd") {
                    roomOccupied[to] = ants[idx].name;
                }

                ants[idx].position = to;
                if (to == "Sd") ants[idx].finished = true;

                cout << ants[idx].name << " moves to " << to << endl;
            }

            cout << endl;
            step++;
        } else {
            break; // No moves possible → stop
        }
    }

    cout << "All ants have reached Sd in " << step - 1 << " steps!" << endl;
    return 0;
}
