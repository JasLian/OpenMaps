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

#include "tinyxml2.h"
#include "dist.h"
#include "graph.h"
#include "osm.h"


using namespace std;
using namespace tinyxml2;

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

void outputBuildings(const BuildingInfo& building1, const BuildingInfo& building2){
    cout << "Person 1's point:\n "
                 << building1.Fullname << endl
                 << " (" << building1.Coords.Lat << "," << building1.Coords.Lon << ")\n";
            
    cout << "Person 2's point:\n "
            << building2.Fullname << endl
            << " (" << building2.Coords.Lat << "," << building2.Coords.Lon << ")\n";
}

//
// Implement your standard application here
//
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
        }
        else if (!build2Found){
            cout << "Person 2's building not found\n";
        }

        if (build1Found && build2Found){
            outputBuildings(building1, building2);
        }
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
