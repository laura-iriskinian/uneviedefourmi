#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <climits>
#include <ctime>
#include <iomanip>

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

// Structure to store ant colony information
struct ColonyInfo {
    int numAnts;
    map<string, int> roomCapacity;
    map<string, vector<string>> tunnels; // Graph of connections
};

// Structure for Dijkstra's algorithm
struct Node {
    string room;
    int distance;
    vector<string> path;
    
    bool operator>(const Node& other) const {
        return distance > other.distance;
    }
};

// Global variables
ColonyInfo colonyInfo;
map<string, int> roomOccupancy;

void addTunnel(const string& a, const string& b) {
    colonyInfo.tunnels[a].push_back(b);
    colonyInfo.tunnels[b].push_back(a);
}

// Read colony file
bool loadColonyFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: unable to open file " << filename << endl;
        return false;
    }

    string line;
    
    // Read number of ants
    if (getline(file, line)) {
        if (line.substr(0, 2) == "f=") {
            colonyInfo.numAnts = stoi(line.substr(2));
        }
    }

    // Read rooms and their capacities
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        // If line contains " - ", it's a tunnel
        if (line.find(" - ") != string::npos) {
            // Process tunnels later
            break;
        }
        
        // Process rooms
        istringstream iss(line);
        string roomName;
        iss >> roomName;
        
        // Default capacity = 1
        int capacity = 1;
        
        // Look for capacity between { }
        size_t start = line.find("{");
        size_t end = line.find("}");
        if (start != string::npos && end != string::npos) {
            string capacityStr = line.substr(start + 1, end - start - 1);
            capacityStr.erase(0, capacityStr.find_first_not_of(" \t"));
            capacityStr.erase(capacityStr.find_last_not_of(" \t") + 1);
            capacity = stoi(capacityStr);
        }
        
        colonyInfo.roomCapacity[roomName] = capacity;
    }

    // Process current line (first tunnel) and following ones
    do {
        if (line.empty()) continue;
        
        size_t pos = line.find(" - ");
        if (pos != string::npos) {
            string room1 = line.substr(0, pos);
            string room2 = line.substr(pos + 3);
            
            // Clean spaces
            room1.erase(0, room1.find_first_not_of(" \t"));
            room1.erase(room1.find_last_not_of(" \t") + 1);
            room2.erase(0, room2.find_first_not_of(" \t"));
            room2.erase(room2.find_last_not_of(" \t") + 1);
            
            addTunnel(room1, room2);
        }
    } while (getline(file, line));

    file.close();
    
    // Initialize room occupancy
    for (const auto& room : colonyInfo.roomCapacity) {
        roomOccupancy[room.first] = 0;
    }
    
    return true;
}

// Dijkstra's algorithm to find shortest path with congestion awareness
vector<string> findShortestPathDijkstra(string start, string target, const map<string, int>& tempOccupancy) {
    if (start == target) return {start};

    priority_queue<Node, vector<Node>, greater<Node>> pq;
    map<string, int> distances;
    map<string, vector<string>> paths;
    map<string, bool> visited;

    // Initialize distances
    for (const auto& room : colonyInfo.roomCapacity) {
        distances[room.first] = INT_MAX;
    }
    distances["Sv"] = 0;
    distances["Sd"] = INT_MAX;

    // Start with the initial room
    pq.push({start, 0, {start}});
    distances[start] = 0;
    paths[start] = {start};

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        if (visited[current.room]) continue;
        visited[current.room] = true;

        if (current.room == target) {
            return current.path;
        }

        // Explore neighbors
        if (colonyInfo.tunnels.find(current.room) != colonyInfo.tunnels.end()) {
            for (const string& neighbor : colonyInfo.tunnels[current.room]) {
                if (visited[neighbor]) continue;

                // Calculate edge weight based on congestion
                int edgeWeight = 1;
                if (neighbor != "Sv" && neighbor != "Sd") {
                    int occupancy = tempOccupancy.count(neighbor) ? tempOccupancy.at(neighbor) : 0;
                    int capacity = colonyInfo.roomCapacity.count(neighbor) ? colonyInfo.roomCapacity.at(neighbor) : 1;
                    
                    // Penalize congested rooms
                    if (occupancy >= capacity) {
                        edgeWeight = 100; // High penalty for full rooms
                    } else if (occupancy > capacity * 0.7) {
                        edgeWeight = 5; // Medium penalty for crowded rooms
                    }
                }

                int newDistance = current.distance + edgeWeight;

                if (newDistance < distances[neighbor]) {
                    distances[neighbor] = newDistance;
                    vector<string> newPath = current.path;
                    newPath.push_back(neighbor);
                    paths[neighbor] = newPath;
                    pq.push({neighbor, newDistance, newPath});
                }
            }
        }
    }

    return {}; // No path found
}

// Function to get adjacent rooms
vector<string> getPossibleNextRooms(string currentPos) {
    vector<string> nextRooms;
    if (colonyInfo.tunnels.find(currentPos) != colonyInfo.tunnels.end()) {
        nextRooms = colonyInfo.tunnels[currentPos];
    }
    return nextRooms;
}

// Choose the best next room using Dijkstra
string chooseBestNextRoom(string currentPos, const map<string, int>& tempOccupancy) {
    vector<string> options = getPossibleNextRooms(currentPos);

    if (options.empty()) return "";

    // If an option is "Sd", prioritize it
    for (const string& room : options) {
        if (room == "Sd") return room;
    }

    // Use Dijkstra to find the best path considering congestion
    vector<string> bestPath = findShortestPathDijkstra(currentPos, "Sd", tempOccupancy);
    
    if (bestPath.size() > 1) {
        string nextRoom = bestPath[1]; // Next room in the optimal path
        
        // Verify the room has space
        int currentOccupancy = tempOccupancy.count(nextRoom) ? tempOccupancy.at(nextRoom) : 0;
        int capacity = colonyInfo.roomCapacity.count(nextRoom) ? colonyInfo.roomCapacity.at(nextRoom) : 1;
        
        if (currentOccupancy < capacity) {
            return nextRoom;
        }
    }

    // Fallback: choose any available adjacent room
    for (const string& room : options) {
        int currentOccupancy = tempOccupancy.count(room) ? tempOccupancy.at(room) : 0;
        int capacity = colonyInfo.roomCapacity.count(room) ? colonyInfo.roomCapacity.at(room) : 1;

        if (currentOccupancy < capacity) {
            return room;
        }
    }

    return "";
}

// Sort room list by numerical order
bool compareRooms(const pair<string, int>& a, const pair<string, int>& b) {
    string roomA = a.first;
    string roomB = b.first;

    if (roomA[0] == 'S' && roomB[0] == 'S') {
        // Extract numbers and compare
        int numA = stoi(roomA.substr(1));
        int numB = stoi(roomB.substr(1));
        return numA < numB;
    }
    return false;
}

// Presentation for results
void printColonyInfo() {
    cout << "+++ Ant Colony Information +++" << endl;

    cout << "Number of ants: " << colonyInfo.numAnts << endl;

    vector<pair<string, int>> sortedRooms;
    for (const auto& room : colonyInfo.roomCapacity) {
        sortedRooms.push_back({room.first, room.second});
    }
    sort(sortedRooms.begin(), sortedRooms.end(), compareRooms);
    cout << "Rooms and capacities:" << endl;
    for (const auto& room : sortedRooms) {
        cout << "  " << room.first << " (capacity " << room.second << ")" << endl;
    }

    cout << "Tunnels:" << endl;
    map<pair<string, string>, bool> printed;
    for (const auto& room : colonyInfo.tunnels) {
        for (const string& neighbor : room.second) {
            pair<string, string> tunnel = {min(room.first, neighbor), max(room.first, neighbor)};
            if (!printed[tunnel]) {
                cout << "  " << room.first << " - " << neighbor << endl;
                printed[tunnel] = true;
            }
        }
    }
    cout << endl;
}

// Main program loop
int main(int argc, char* argv[]) {
    // Démarrer le chronométrage total
    clock_t start_total = clock();
    
    string filename;
    
    if (argc > 1) {
        filename = argv[1];
    } else {
        cout << "Enter the ant colony filename: ";
        cin >> filename;
    }

    // Chronométrer le chargement du fichier
    clock_t start_load = clock();
    if (!loadColonyFromFile(filename)) {
        return 1;
    }
    clock_t end_load = clock();
    double load_time = double(end_load - start_load) / CLOCKS_PER_SEC;

    printColonyInfo();

    // Create ants
    vector<Ant> ants;
    for (int i = 1; i <= colonyInfo.numAnts; ++i) {
        ants.push_back(Ant("f" + to_string(i)));
    }

    cout << "Starting simulation with " << ants.size() << " ants (using Dijkstra algorithm)" << endl;
    cout << endl;

    // Chronométrer la simulation
    clock_t start_simulation = clock();
    
    int step = 1;
    bool allFinished = false;

    while (!allFinished && step <= 50) { // Safety limit
        allFinished = true;

        vector<pair<int, string>> plannedMoves;
        map<string, int> tempOccupancy = roomOccupancy;

        // Sort ants by priority using Dijkstra distances
        vector<int> antOrder;
        for (int i = 0; i < ants.size(); i++) {
            if (!ants[i].finished) {
                antOrder.push_back(i);
            }
        }

        // Prioritize ants with shorter Dijkstra distances to Sd
        sort(antOrder.begin(), antOrder.end(), [&](int a, int b) {
            vector<string> pathA = findShortestPathDijkstra(ants[a].position, "Sd", tempOccupancy);
            vector<string> pathB = findShortestPathDijkstra(ants[b].position, "Sd", tempOccupancy);

            if (pathA.size() == pathB.size()) {
                return a < b; // f1 before f2, etc.
            }
            return pathA.size() < pathB.size(); // Shortest path first
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

    clock_t end_simulation = clock();
    clock_t end_total = clock();
    
    // Calculer les temps d'exécution
    double simulation_time = double(end_simulation - start_simulation) / CLOCKS_PER_SEC;
    double total_time = double(end_total - start_total) / CLOCKS_PER_SEC;

    cout << "All ants have reached Sd in " << step - 1 << " steps!" << endl;
    cout << endl;
    
    // Afficher les statistiques de performance
    cout << "+++ Performance Statistics (Dijkstra Algorithm) +++" << endl;
    cout << fixed << setprecision(6);
    cout << "File loading time: " << load_time << " seconds" << endl;
    cout << "Simulation time: " << simulation_time << " seconds" << endl;
    cout << "Total execution time: " << total_time << " seconds" << endl;
    cout << "Average time per step: " << (step > 1 ? simulation_time / (step - 1) : 0) << " seconds" << endl;
    
    return 0;
}