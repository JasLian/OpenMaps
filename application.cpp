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
                                     set<string>& usedBuildings){

    Coordinates center = centerBetween2Points(building1.Coords.Lat, building1.Coords.Lon, building2.Coords.Lat, building2.Coords.Lon);
    BuildingInfo centerBuilding = Buildings.at(0);
    double closestDist = distBetween2Points(centerBuilding.Coords.Lat, centerBuilding.Coords.Lon, center.Lat, center.Lon);

    double dist;
    for (const BuildingInfo& building : Buildings){

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
        BuildingInfo destination = findDestinationBuilding(Buildings, building1, building2, usedBuildings);

        vector<long long> nearestNodes;
        findNearestNodes(Footways, Nodes, building1, building2, destination, nearestNodes);

        outputBuildings(building1, building2, destination);
        cout << endl;
        outputClosestNodes(nearestNodes, Nodes);

        //
        // TO DO: lookup buildings, find nearest start and dest nodes, find center
        // run Dijkstra's alg from each start, output distances and paths to destination:
        //


        // cout << "Person 1's building not found" << endl;
        // cout << "Person 2's building not found" << endl;


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
