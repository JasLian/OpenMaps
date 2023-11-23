// application.cpp <Starter Code>
// <Your name>
//
//
// Adam T Koehler, PhD
// University of Illinois Chicago
// CS 251, Fall 2023
//
// Project Original Variartion By:
// Joe Hummel, PhD
// University of Illinois at Chicago
//
// 
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <queue>

#include "tinyxml2.h"
#include "dist.h"
#include "graph.h"
#include "osm.h"


using namespace std;
using namespace tinyxml2;

const double INF = numeric_limits<double>::max();

class prioritize {
    public:
        bool operator()(const pair<long long, double>& p1, const pair<long long, double>& p2) const
        {
            return p1.second < p2.second; 
        }
};

void findBuildings(const vector<BuildingInfo>& Buildings, 
                          string person1Building, string person2Building, 
                          BuildingInfo& building1, BuildingInfo& building2,
                          bool& build1Found, bool& build2Found){

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

BuildingInfo findDestinationBuilding(const vector<BuildingInfo>& Buildings, 
                                     const BuildingInfo& building1, const BuildingInfo& building2,
                                     set<string>& usedBuildings, set<string> unreachableBuildings){

    Coordinates center = centerBetween2Points(building1.Coords.Lat, building1.Coords.Lon, building2.Coords.Lat, building2.Coords.Lon);
    BuildingInfo centerBuilding = Buildings.at(0);
    double closestDist = distBetween2Points(centerBuilding.Coords.Lat, centerBuilding.Coords.Lon, center.Lat, center.Lon);

    double dist;
    for (const BuildingInfo& building : Buildings){

        if (usedBuildings.count(building.Fullname) || unreachableBuildings.count(building.Fullname)) continue;

        dist = distBetween2Points(building.Coords.Lat, building.Coords.Lon, center.Lat, center.Lon);
        
        if (dist < closestDist){
            centerBuilding = building;
            closestDist = dist;
        }
        
    }

    usedBuildings.emplace(centerBuilding.Fullname);
    return centerBuilding;
    
}

void outputBuildings(const BuildingInfo& building1, const BuildingInfo& building2, const BuildingInfo& dest){
    cout << "Person 1's point:\n "
                 << building1.Fullname << endl
                 << " (" << building1.Coords.Lat << ", " << building1.Coords.Lon << ")\n";
            
    cout << "Person 2's point:\n "
            << building2.Fullname << endl
            << " (" << building2.Coords.Lat << ", " << building2.Coords.Lon << ")\n";

    cout << "Nearest destination node\n "
            << dest.Fullname << endl
            << " (" << dest.Coords.Lat << ", " << dest.Coords.Lon << ")\n";
        
}

void findNearestNodes(vector<FootwayInfo>& Footways, map<long long, Coordinates>& allNodes,
                      const BuildingInfo building1, const BuildingInfo building2, const BuildingInfo center, 
                      vector<long long>& closestNodes){

    long long closestNode1, closestNode2, closestNode3;
    double closestDist1 = distBetween2Points(building1.Coords.Lat, building1.Coords.Lon, allNodes[Footways.at(0).Nodes.at(0)].Lat, allNodes[Footways.at(0).Nodes.at(0)].Lon),
           closestDist2 = distBetween2Points(building2.Coords.Lat, building2.Coords.Lon, allNodes[Footways.at(0).Nodes.at(0)].Lat, allNodes[Footways.at(0).Nodes.at(0)].Lon),
           closestDist3 = distBetween2Points(center.Coords.Lat, center.Coords.Lon, allNodes[Footways.at(0).Nodes.at(0)].Lat, allNodes[Footways.at(0).Nodes.at(0)].Lon);

    double dist;
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

void outputClosestNodes(vector<long long>& closestNodes, map<long long, Coordinates>& Nodes){

    cout << "Nearest P1 node:\n"
         << " " << closestNodes.at(0) << endl
         << " (" << Nodes[closestNodes.at(0)].Lat << ", " << Nodes[closestNodes.at(0)].Lon << ")\n";

    cout << "Nearest P1 node:\n"
         << " " << closestNodes.at(1) << endl
         << " (" << Nodes[closestNodes.at(1)].Lat << ", " << Nodes[closestNodes.at(1)].Lon << ")\n";

    cout << "Nearest P1 node:\n"
         << " " << closestNodes.at(2) << endl
         << " (" << Nodes[closestNodes.at(2)].Lat << ", " << Nodes[closestNodes.at(2)].Lon << ")\n";

}

void dijkstra(const long long& start, const graph<long long, double>& graph, 
              map<long long, long long>& predecessors, map<long long, double>& distance){

    vector<long long> vertices = graph.getVertices();
    priority_queue<pair<long long, double>, vector<pair<long long, double>>, prioritize> unvisitedQueue;
    for (long long& vertex : vertices){
        distance[vertex] = INF;
        predecessors[vertex] = -1;
        unvisitedQueue.push(pair(vertex, INF));
    }
    distance[start] = 0;
    unvisitedQueue.push(pair(start, 0));

    set<long long> visited;

    while (!unvisitedQueue.empty()){
        pair<long long, double> current = unvisitedQueue.top();
        unvisitedQueue.pop();

        if (current.second == INF){
            break;
        }
        else if (visited.count(current.first)){
            continue;
        }
        
        set<long long> neighbors = graph.neighbors(current.first);
        double currentDistance = INF;
        for (const long long& vertex : neighbors){
            graph.getWeight(current.first, vertex, currentDistance);

            double currentTotalDistance = distance[vertex];
            double altTotalDistance = (currentTotalDistance != INF) ? currentTotalDistance + currentDistance : currentDistance;

            if (altTotalDistance < currentTotalDistance || currentTotalDistance == INF){
                distance[vertex] = altTotalDistance;
                predecessors[vertex] = current.first;
                unvisitedQueue.push(pair(vertex, altTotalDistance));
            }
        }

    }

} 

bool buildPath(long long& destination, vector<long long> path, double totDistance, 
               map<long long, long long>& predecessors, 
               map<long long, double>& distance){

    if (distance[destination] == INF) return false;

    return true;
}

void application(
    map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways,
    vector<BuildingInfo>& Buildings, graph<long long, double>& G) {

    string person1Building, person2Building;

    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);

    while (person1Building != "#") {
        cout << "Enter person 2's building (partial name or abbreviation)> ";
        getline(cin, person2Building);

        BuildingInfo building1, building2;
        bool build1Found = false, build2Found = false;
        findBuildings(Buildings, person1Building, person2Building, building1, building2, build1Found, build2Found);

        if (!build1Found){
            cout << "Person 1's building not found\n";
            continue;
        }
        else if (!build2Found){
            cout << "Person 2's building not found\n";
            continue;
        }

        set<string> usedBuildings;
        set<string> unreachableBuildings;
        BuildingInfo destination;
        vector<long long> nearestNodes;

        map<long long, long long> predecessors;
        map<long long, double> distance; 
        vector<long long> path1, path2;
        bool reachable = true, destReach1 = true, destReach2 = false;
        double path1Distance, path2Distance;

        do{
            
            nearestNodes.clear();
            destination = findDestinationBuilding(Buildings, building1, building2, usedBuildings, unreachableBuildings);
            findNearestNodes(Footways, Nodes, building1, building2, destination, nearestNodes);

            dijkstra(nearestNodes.at(0), G, predecessors, distance);
            vector<long long> reachablePath;
            double reachableDistance = INF;
            // reachable = buildPath(nearestNodes.at(1), reachablePath, reachableDistance, predecessors, distance);
            if (!reachable) break;


            dijkstra(nearestNodes.at(0), G, predecessors, distance);
            // destReach1 = buildPath(nearestNodes.at(2), reachablePath, path1Distance, predecessors, distance);
            if (!destReach1) continue;

            dijkstra(nearestNodes.at(1), G, predecessors, distance);
            // destReach2 = buildPath(nearestNodes.at(2), reachablePath, path2Distance, predecessors, distance);
            if (!destReach2) continue;


        } while (destReach1 && destReach2);
        

        outputBuildings(building1, building2, destination);
        cout << endl;
        outputClosestNodes(nearestNodes, Nodes);

        if (!reachable){
            cout << "Sorry, destination unreachable.\n";
        }

        //
        // another navigation?
        //
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


    //
    // TO DO: build the graph, output stats:
    //

    for (auto& pair : Nodes){
        G.addVertex(pair.first);
    }

    for (FootwayInfo& path : Footways){

        int nodeCount = path.Nodes.size();

        for (int i = 0; i < nodeCount - 1; i++){

            long long node1 = path.Nodes.at(i), node2 = path.Nodes.at(i + 1);
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
