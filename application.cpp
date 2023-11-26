// application.cpp
// Jason Liang
//
// main file for the program. Contains the main and helper functions to perform the path finding functionality of the program

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <queue>
#include <stack>
#include <fstream>

#include "tinyxml2.h"
#include "dist.h"
#include "graph.h"
#include "osm.h"


using namespace std;
using namespace tinyxml2;

const double INF = numeric_limits<double>::max();

// prioritize class used for priority queue ordering
class prioritize {
    public:
        bool operator()(const pair<long long, double>& p1, const pair<long long, double>& p2) const
        {
            return p1.second > p2.second; 
        }
};

/*fucntion finds the buildings that matches the names or abbreviations given by the user
if a building is found, its corresponding BuildingInfo parameter is changed
Takes 7 parameters:
    1. Buildings: a vector of buildings to search from
    2, 3. person1Building, person2Building: the names or abbreviations given
    4, 5. building1, building2: the BuildingInfo structs storing the found buildings
    6, 7. build1Found, build2Found: flag variables keeping track if a building is found
No returns*/
void findBuildings(const vector<BuildingInfo>& Buildings, 
                   const string person1Building, const string person2Building, 
                   BuildingInfo& building1, BuildingInfo& building2,
                   bool& build1Found, bool& build2Found){

    // first, attempt to search by abbreviation
    for (BuildingInfo building : Buildings){

        if (building.Abbrev == person1Building && !build1Found){
            building1 = building;
            build1Found = true;
        }

        if (building.Abbrev == person2Building && !build2Found){
            building2 = building;
            build2Found = true;
        }
    }

    //if either building could not be found via its abbreviation, search by partial or full name
    if (!build1Found || !build2Found){

        for (BuildingInfo building : Buildings){

            if (building.Fullname.find(person1Building) != string::npos && !build1Found){
                building1 = building;
                build1Found = true;
            }

            if (building.Fullname.find(person2Building) != string::npos && !build2Found){
                building2 = building;
                build2Found = true;
            }
        }
    }
}

/*function finds the destination/center building of two buildings
Takes 4 parameters:
    1. Buildings: a vector of buildings to search from
    2, 3. building1, building2: the starting buildings
    4. usedBuildings: a set of buildings already chosen and tried as the destination
returns the destination building*/
BuildingInfo findDestinationBuilding(const vector<BuildingInfo>& Buildings, 
                                     const BuildingInfo& building1, const BuildingInfo& building2,
                                     set<string>& usedBuildings){

    // calculates the center between building1 and building2
    Coordinates center = centerBetween2Points(building1.Coords.Lat, building1.Coords.Lon, building2.Coords.Lat, building2.Coords.Lon);

    // searches through Buildings to find the building closest to the center
    BuildingInfo centerBuilding = Buildings.at(0);
    double closestDist = distBetween2Points(centerBuilding.Coords.Lat, centerBuilding.Coords.Lon, center.Lat, center.Lon);
    double dist;
    for (const BuildingInfo& building : Buildings){

        // buildings already used are skipped
        if (usedBuildings.count(building.Fullname)) continue;

        dist = distBetween2Points(building.Coords.Lat, building.Coords.Lon, center.Lat, center.Lon);
        
        if (dist < closestDist){
            centerBuilding = building;
            closestDist = dist;
        }
        
    }

    usedBuildings.emplace(centerBuilding.Fullname);
    return centerBuilding;
    
}

/*function outputs the building informations
Takes 3 parameters:
    1 - 3. building1, building2, dest: the 3 buildings 
No returns*/
void outputBuildings(const BuildingInfo& building1, const BuildingInfo& building2, const BuildingInfo& dest){
    cout << "Person 1's point:\n "
                 << building1.Fullname << endl
                 << " (" << building1.Coords.Lat << ", " << building1.Coords.Lon << ")\n";
            
    cout << "Person 2's point:\n "
            << building2.Fullname << endl
            << " (" << building2.Coords.Lat << ", " << building2.Coords.Lon << ")\n";

    cout << "Destination Building:\n "
            << dest.Fullname << endl
            << " (" << dest.Coords.Lat << ", " << dest.Coords.Lon << ")\n";
        
}

/*function finds the closest nodes on a footway to each of the 2 starting buildings and destination building
Takes 6 parameters:
    1. Footways: the vector of footways
    2. allNodes: the map of all nodes on the map/graph
    3 - 5. building1, building2, center: the 3 buildings
    6. closestNodes: the vector to store the 3 nodes
No returns*/
void findNearestNodes(vector<FootwayInfo>& Footways, map<long long, Coordinates>& allNodes,
                      const BuildingInfo building1, const BuildingInfo building2, const BuildingInfo center, 
                      vector<long long>& closestNodes){

    long long closestNode1, closestNode2, closestNode3;
    double closestDist1 = distBetween2Points(building1.Coords.Lat, building1.Coords.Lon, allNodes[Footways.at(0).Nodes.at(0)].Lat, allNodes[Footways.at(0).Nodes.at(0)].Lon),
           closestDist2 = distBetween2Points(building2.Coords.Lat, building2.Coords.Lon, allNodes[Footways.at(0).Nodes.at(0)].Lat, allNodes[Footways.at(0).Nodes.at(0)].Lon),
           closestDist3 = distBetween2Points(center.Coords.Lat, center.Coords.Lon, allNodes[Footways.at(0).Nodes.at(0)].Lat, allNodes[Footways.at(0).Nodes.at(0)].Lon);

    double dist;

    // finds the closest node on a footway from each building
    for (const FootwayInfo& way : Footways){

        for (const long long& node : way.Nodes){

            dist = distBetween2Points(building1.Coords.Lat, building1.Coords.Lon, allNodes[node].Lat, allNodes[node].Lon);
            if (dist < closestDist1){
                closestDist1 = dist;
                closestNode1 = node;
            }

            dist = distBetween2Points(building2.Coords.Lat, building2.Coords.Lon, allNodes[node].Lat, allNodes[node].Lon);
            if (dist < closestDist2){
                closestDist2 = dist;
                closestNode2 = node;
            }

            dist = distBetween2Points(center.Coords.Lat, center.Coords.Lon, allNodes[node].Lat, allNodes[node].Lon);
            if (dist < closestDist3){
                closestDist3 = dist;
                closestNode3 = node;
            }
        }
    
    }

    closestNodes.push_back(closestNode1);
    closestNodes.push_back(closestNode2);
    closestNodes.push_back(closestNode3);
}

/*function outputs the nodes information
Takes 2 parameters:
    1. closestNodes: the vector storing the closest nodes
    2. Nodes: the map storing information on each node
No returns*/
void outputClosestNodes(vector<long long>& closestNodes, map<long long, Coordinates>& Nodes){

    cout << "Nearest P1 node:\n"
         << " " << closestNodes.at(0) << endl
         << " (" << Nodes[closestNodes.at(0)].Lat << ", " << Nodes[closestNodes.at(0)].Lon << ")\n";

    cout << "Nearest P2 node:\n"
         << " " << closestNodes.at(1) << endl
         << " (" << Nodes[closestNodes.at(1)].Lat << ", " << Nodes[closestNodes.at(1)].Lon << ")\n";

    cout << "Nearest destination node:\n"
         << " " << closestNodes.at(2) << endl
         << " (" << Nodes[closestNodes.at(2)].Lat << ", " << Nodes[closestNodes.at(2)].Lon << ")\n";

}

/*function performs Dijkstra's algorithm to find the shortest path between 2 nodes
Takes 4 parameters:
    1. start: the node the search starts from
    2. graph: the graph being traversed
    3. predecessors: the map to store the predecessors of each node
    4. distance: the map the store the shortest distance each node is from the start
No returns*/
void dijkstra(const long long& start, const graph<long long, double>& graph, 
              map<long long, long long>& predecessors, map<long long, double>& distance){

    // first intitalizes the distance of each node to INF, and pushes them to the priority queue
    vector<long long> vertices = graph.getVertices();
    priority_queue<pair<long long, double>, vector<pair<long long, double>>, prioritize> unvisitedQueue;
    for (long long& vertex : vertices){
        distance[vertex] = INF;
        predecessors[vertex] = -1;
        unvisitedQueue.push(pair(vertex, INF));
    }
    
    // sets start's distance away from start to 0 and pushses it to the priority queue
    distance[start] = 0;
    unvisitedQueue.push(pair(start, 0));

    set<long long> visited;
    
    while (!unvisitedQueue.empty()){

        //dequeues off the first node in the queue
        pair<long long, double> current = unvisitedQueue.top();
        unvisitedQueue.pop();

        // checks if the node should be checked
        if (current.second == INF){
            break;
        }
        else if (visited.count(current.first)){
            continue;
        }

        visited.emplace(current.first);
        
        set<long long> neighbors = graph.neighbors(current.first);

        double currentDistance = INF;

        // visits every neighboring node and checks if a new shortest distance from start is found 
        for (const long long& vertex : neighbors){

            graph.getWeight(current.first, vertex, currentDistance);
            double currentTotalDistance = distance[vertex];
            double altTotalDistance = current.second + currentDistance;

            if (altTotalDistance < currentTotalDistance){
                distance[vertex] = altTotalDistance;
                predecessors[vertex] = current.first;
                unvisitedQueue.push(pair(vertex, altTotalDistance));
            }

        }

    }

} 

/*function builds the path based on the results of the Dijkstra's algorithm results for predecessors and distance
Takes 5 parameters:
    1. destination: the destination for the path
    2. path: the stack to store the path
    3. totDistance: if a path is possible, its value is changed to the total distance of that path
    4. predecessors: the map storing the predecessors of each node
    5. distance: the map storing the distances of each node from the start
Returns a boolean value if a path is possible*/
bool buildPath(long long& destination, stack<long long>& path, double& totDistance, 
               map<long long, long long>& predecessors, 
               map<long long, double>& distance){

    totDistance = distance[destination];

    // checks if a path is possible by checking if totDistance is equal to INF
    if (totDistance == INF) return false;

    path.push(destination);
    long long predNode = predecessors[destination];

    // pushes to the path stack to build the actual path
    while (predNode != -1){
        path.push(predNode);
        predNode = predecessors[predNode];
    }

    return true;
}

/*function prints out the path
Takes 1 parameters:
    path: the stack storing the ath
No returns*/
void printPath(stack<long long> path){

    cout << "Path: " << path.top();
    path.pop();

    while (!path.empty()){
        cout << "->" << path.top();
        path.pop();
    }

    cout << endl;

}

/*main driver function for program.
reads in inputs for the 2 starting buildings, finds their center, and finds a path to the center
Takes 4 parameters:
    1. Nodes: the map storing all the nodes on the graph
    2. Footways: the vector of all footways
    3. Buildings: the vector of all buildings
    4. G: the graph representing the map
No returns*/
void application(
    map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways,
    vector<BuildingInfo>& Buildings, graph<long long, double>& G) {

    string person1Building, person2Building;

    // reads in starting buildings
    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);

    while (person1Building != "#") {
        cout << "Enter person 2's building (partial name or abbreviation)> ";
        getline(cin, person2Building);

        BuildingInfo building1, building2;
        bool build1Found = false, build2Found = false;

        //finds starting buildings
        findBuildings(Buildings, person1Building, person2Building, building1, building2, build1Found, build2Found);

        if (!build1Found){
            cout << "Person 1's building not found\n";
        }
        else if (!build2Found){
            cout << "Person 2's building not found\n";
        }
        else{
            set<string> usedBuildings;
            BuildingInfo destination;
            vector<long long> nearestNodes;

            map<long long, long long> predecessors;
            map<long long, double> distance; 
            stack<long long> path1, path2;
            bool reachable = false, destReach1 = false, destReach2 = false;
            double path1Distance = INF, path2Distance = INF;

            // do-while loop to repeatedly find a destination building that is reachable from the two starting buildings
            do{
                
                nearestNodes.clear();
                destination = findDestinationBuilding(Buildings, building1, building2, usedBuildings);
                findNearestNodes(Footways, Nodes, building1, building2, destination, nearestNodes);

                dijkstra(nearestNodes.at(0), G, predecessors, distance);
                stack<long long> reachablePath;
                double reachableDistance = INF;
                reachable = buildPath(nearestNodes.at(1), reachablePath, reachableDistance, predecessors, distance);

                //if a path from building1 to building2 does not exist, immediately stop searching
                if (!reachable) break;
                predecessors.clear();
                distance.clear();

                dijkstra(nearestNodes.at(0), G, predecessors, distance);
                destReach1 = buildPath(nearestNodes.at(2), path1, path1Distance, predecessors, distance);
                if (!destReach1) continue;
                predecessors.clear();
                distance.clear();

                dijkstra(nearestNodes.at(1), G, predecessors, distance);
                destReach2 = buildPath(nearestNodes.at(2), path2, path2Distance, predecessors, distance);
                if (!destReach2) continue;
                predecessors.clear();
                distance.clear();

            } while (!destReach1 && !destReach2);
            
            // outputs the buildings and nearest nodes to said buildings
            outputBuildings(building1, building2, destination);
            cout << endl;
            outputClosestNodes(nearestNodes, Nodes);

            // messages for when a path is found or not
            if (!reachable){
                cout << "\nSorry, destination unreachable.\n";
            }
            else{
                cout << "\nPerson 1's distance to dest: " << path1Distance << " miles\n";
                printPath(path1);

                cout << "\nPerson 2's distance to dest: " << path2Distance << " miles\n";
                printPath(path2);
            }
        }
        

        cout << endl;
        cout << "Enter person 1's building (partial name or abbreviation), or #> ";
        getline(cin, person1Building);
    }    
}

int main() {
    graph<long long, double> G;

    // maps a Node ID to it's coordinates (lat, lon)
    map<long long, Coordinates>  Nodes;
    // info about each footway, in no particular order
    vector<FootwayInfo>          Footways;
    // info about each building, in no particular order
    vector<BuildingInfo>         Buildings;
    XMLDocument                  xmldoc;

    cout << "** Navigating UIC open street map **" << endl;
    cout << endl;
    cout << std::setprecision(8);

    string def_filename = "map.osm";
    string filename;

    cout << "Enter map filename> ";
    getline(cin, filename);

    if (filename == "") {
        filename = def_filename;
    }

    //
    // Load XML-based map file
    //
    if (!LoadOpenStreetMap(filename, xmldoc)) {
        cout << "**Error: unable to load open street map." << endl;
        cout << endl;
        return 0;
    }

    //
    // Read the nodes, which are the various known positions on the map:
    //
    int nodeCount = ReadMapNodes(xmldoc, Nodes);

    //
    // Read the footways, which are the walking paths:
    //
    int footwayCount = ReadFootways(xmldoc, Footways);

    //
    // Read the university buildings:
    //
    int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

    //
    // Stats
    //
    assert(nodeCount == (int)Nodes.size());
    assert(footwayCount == (int)Footways.size());
    assert(buildingCount == (int)Buildings.size());

    cout << endl;
    cout << "# of nodes: " << Nodes.size() << endl;
    cout << "# of footways: " << Footways.size() << endl;
    cout << "# of buildings: " << Buildings.size() << endl;

    // loops through Nodes and adds each node to G as a vertex
    for (auto& pair : Nodes){
        G.addVertex(pair.first);
    }

    // loops through Footways, taking each footway and adding each pair of nodes as an edge to G
    for (FootwayInfo& footway : Footways){

        int nodeCount = footway.Nodes.size();

        for (int i = 0; i < nodeCount - 1; i++){

            long long node1 = footway.Nodes.at(i), node2 = footway.Nodes.at(i + 1);
            double dist = distBetween2Points(Nodes[node1].Lat, Nodes[node1].Lon, Nodes[node2].Lat, Nodes[node2].Lon);

            if (!G.addEdge(node1, node2, dist)){
                cout << "Unable to add path from " << node1 << " to " << node2 << "(" << dist << ")\n";
            }

            if (!G.addEdge(node2, node1, dist)){
                cout << "Unable to add path from " << node2 << " to " << node1 << "(" << dist << ")\n";
            }

        }
    }


    cout << "# of vertices: " << G.NumVertices() << endl;
    cout << "# of edges: " << G.NumEdges() << endl;
    cout << endl;

    // Execute Application
    application(Nodes, Footways, Buildings, G);

    //
    // done:
    //
    cout << "** Done **" << endl;
    return 0;
}
