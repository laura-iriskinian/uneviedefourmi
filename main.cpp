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

class Ant {
public:
    string name;
    vector<string> path; // chemin complet à suivre
    int positionIndex;   // index dans le chemin
    bool finished;

    Ant(string n, const vector<string>& p) : name(n), path(p), positionIndex(0), finished(false) {}
};

map<string, vector<string>> colony;
map<string, int> roomCapacity;
map<string, vector<string>> roomOccupants;
vector<Ant> ants;

// Lecture fichier
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
        if (cap == -1) cap = numeric_limits<int>::max();
        roomCapacity[name] = cap;
        roomOccupants[name] = {};
    }

    file >> numTunnels;
    for (int i = 0; i < numTunnels; ++i) {
        string a, b;
        file >> a >> b;
        colony[a].push_back(b);
        colony[b].push_back(a);

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

// BFS qui retourne tous les plus courts chemins entre start et end
void findAllShortestPaths(const string& start, const string& end, vector<vector<string>>& allPaths) {
    queue<vector<string>> q;
    q.push({start});

    int shortestLength = -1;
    set<string> visited;
    vector<string> currentLevelVisited;

    while (!q.empty()) {
        vector<string> path = q.front();
        q.pop();

        if (shortestLength != -1 && (int)path.size() > shortestLength) break;

        string last = path.back();
        if (last == end) {
            allPaths.push_back(path);
            shortestLength = (int)path.size();
        } else {
            for (const string& neighbor : colony[last]) {
                if (find(path.begin(), path.end(), neighbor) == path.end()) { // éviter cycle
                    vector<string> newPath = path;
                    newPath.push_back(neighbor);
                    q.push(newPath);
                }
            }
        }
    }
}

// Vérifie si on peut entrer dans une salle
bool canEnter(const string& room, const map<string, int>& futureOccupantsCount) {
    if (room == "Sv" || room == "Sd") return true;
    return futureOccupantsCount.at(room) < roomCapacity[room];
}

int main() {
    int numAnts;
    if (!readInput("input.txt", numAnts)) return 1;

    // Trouver tous les plus courts chemins Sv -> Sd
    vector<vector<string>> shortestPaths;
    findAllShortestPaths("Sv", "Sd", shortestPaths);

    if (shortestPaths.empty()) {
        cerr << "[ERROR] No path from Sv to Sd found." << endl;
        return 1;
    }

    cout << "[INFO] Found " << shortestPaths.size() << " shortest paths from Sv to Sd." << endl;
    for (const auto& p : shortestPaths) {
        for (const auto& room : p) cout << room << " ";
        cout << endl;
    }

    // Créer fourmis et leur assigner un chemin (distribution équitable)
    for (int i = 0; i < numAnts; ++i) {
        const vector<string>& path = shortestPaths[i % shortestPaths.size()];
        ants.emplace_back("f" + to_string(i + 1), path);
        roomOccupants["Sv"].push_back(ants.back().name);
    }

    int step = 1;
    bool allFinished = false;

    while (!allFinished) {
        allFinished = true;
        bool anyMove = false;

        // Compte des occupants futurs pour éviter dépassement
        map<string, int> futureOccupantsCount;
        for (auto& [room, occupants] : roomOccupants) {
            futureOccupantsCount[room] = occupants.size();
        }

        // Préparer déplacements
        vector<pair<int, string>> plannedMoves; // {indiceFourmi, prochaineSalle}

        for (int i = 0; i < (int)ants.size(); ++i) {
            Ant& ant = ants[i];
            if (ant.finished) continue;
            allFinished = false;

            int currPos = ant.positionIndex;
            if (currPos + 1 >= (int)ant.path.size()) {
                // déjà à Sd
                ant.finished = true;
                continue;
            }

            string nextRoom = ant.path[currPos + 1];

            // Vérifier capacité et si on peut entrer
            if (canEnter(nextRoom, futureOccupantsCount)) {
                plannedMoves.push_back({i, nextRoom});
                futureOccupantsCount[nextRoom]++;
                futureOccupantsCount[ant.path[currPos]]--;
                anyMove = true;
            }
            // sinon, la fourmi attend
        }

        // Exécuter déplacements
        if (anyMove) {
            cout << "+++ Step " << step << " +++" << endl;
            for (auto& move : plannedMoves) {
                int idx = move.first;
                Ant& ant = ants[idx];
                string from = ant.path[ant.positionIndex];
                string to = move.second;

                // Mettre à jour position fourmi
                ant.positionIndex++;
                ant.finished = (to == "Sd");

                // Mise à jour occupants
                auto& fromList = roomOccupants[from];
                fromList.erase(remove(fromList.begin(), fromList.end(), ant.name), fromList.end());
                roomOccupants[to].push_back(ant.name);

                cout << ant.name << " moves to " << to << endl;
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
