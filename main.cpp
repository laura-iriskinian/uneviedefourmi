#include "ants.hpp"

// Main program loop
int main(int argc, char* argv[]) {
    string filename;

    if (argc > 1) {
        filename = argv[1];
    } else {
        cout << "Enter the ant colony filename: ";
        cin >> filename;
    }

    if (!loadColonyFromFile(filename)) {
        return 1;
    }

    printColonyInfo();

    // Create ants
    vector<Ant> ants;
    for (int i = 1; i <= colonyInfo.numAnts; ++i) {
        ants.push_back(Ant("f" + to_string(i)));
    }

    cout << "Starting simulation with " << ants.size() << " ants" << endl;
    cout << endl;

    int step = 1;
    bool allFinished = false;

    while (!allFinished && step <= 50) { // Safety limit
        allFinished = true;

        vector<pair<int, string>> plannedMoves;
        map<string, int> tempOccupancy = roomOccupancy;

        // Sort ants by priority
        vector<int> antOrder;
        for (int i = 0; i < ants.size(); i++) {
            if (!ants[i].finished) {
                antOrder.push_back(i);
            }
        }

        // Prioritize ants closest to Sd
        sort(antOrder.begin(), antOrder.end(), [&](int a, int b) {
            vector<string> pathA = findShortestPath(ants[a].position, "Sd");
            vector<string> pathB = findShortestPath(ants[b].position, "Sd");

            if (pathA.size() == pathB.size()) {
                return a < b; // if both ants at same distance, f1 before f2, etc.
            }
            return pathA.size() < pathB.size(); // Closest ant to Sd moves first
        });

        // Plan next moves
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
            cout << "+++ Step " << step << " +++" << endl;

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