// graph.h - final implementation
// Jasoon Liang
//
// Basic graph class graph using adjaceny list representation.  
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
    private:
    
        //Basic struct representating an edge in a graph
        struct Edge{
            WeightT edgeWeight; //edge's weight
            VertexT vertexId; //vertex linked to the original vertex
            Edge* next = nullptr; //linked list for adjaceny list implementation
            
        };

        map<VertexT, Edge*> adjList; //map storing the adjaceny list
        int totEdges = 0;

        //
        // _LookupVertex
        // searches if a vertex already exists by searcing through the map
        //
        int _LookupVertex(VertexT v) const {
            return adjList.count(v);
        }


    public:

        //
        // default constructor:
        //
        graph() {}

        //copy constructor:
        //creates a deep copy of an existing graph
        graph(const graph& other){

            this->~graph(); //destroys/clears the current graph

            //gets all the vertex from the other graph, and adds them to the current graph
            vector<VertexT> vertices = other.getVertices();
            for (VertexT vertex : vertices){
                this->addVertex(vertex);
            }

            //finds all the neighbors of each vertex, and adds the edges to the graph
            for (VertexT vertex : vertices){

                set<VertexT> neighbors = other.neighbors(vertex);

                for (VertexT n : neighbors){
                    WeightT weight;
                    other.getWeight(vertex, n, weight);

                    this->addEdge(vertex, n, weight);
                }
            }
        }

        //assignment operator:
        //creates a deep copy of an existing graph
        graph operator=(const graph& other){

            this->~graph(); //destroys/clears the current graph

            //gets all the vertex from the other graph, and adds them to the current graph
            vector<VertexT> vertices = other.getVertices();
            for (VertexT vertex : vertices){
                this->addVertex(vertex);
            }

            //finds all the neighbors of each vertex, and adds the edges to the graph
            for (VertexT vertex : vertices){

                set<VertexT> neighbors = other.neighbors(vertex);

                for (VertexT n : neighbors){
                    WeightT weight;
                    other.getWeight(vertex, n, weight);

                    this->addEdge(vertex, n, weight);
                }
            }

            return *(this);
        }

        // destructor:
        // frees all allocated memory of each Edge node
        ~graph(){
            for (auto& pair : this->adjList){

                Edge* currEdge = pair.second;
                Edge* temp;

                while (currEdge){
                    
                    temp = currEdge;
                    currEdge = currEdge->next;

                    delete temp;
                }
            }

            this->adjList.clear();
            this->totEdges = 0;

        }

        //
        // NumVertices
        //
        // Returns the # of vertices currently in the graph.
        //
        int NumVertices() const {
            return static_cast<int>(this->adjList.size());
        }

        //
        // NumEdges
        //
        // Returns the # of edges currently in the graph.
        //
        int NumEdges() const {
            return this->totEdges;
        }

        //
        // addVertex
        //
        // Adds the vertex v to the graph if there's room, and if so
        // returns true.
        //
        bool addVertex(VertexT v) {

            // checks if the vertex already exists to avoid inserting duplicates
            if (_LookupVertex(v) == 1) {
                return false;
            }

            this->adjList.emplace(v, nullptr); // inserts the vertex

            return true;
        }

        //
        // addEdge
        //
        // Adds the edge (from, to, weight) to the graph, and returns
        // true.  If the vertices do not exist, false is returned.
        //
        // if the edge already exists, the existing edge weight
        // is overwritten with the new edge weight.
        //
        bool addEdge(VertexT from, VertexT to, WeightT weight) {
            
            if (!_LookupVertex(from)) {  // from vertex not found:
                return false;
            }

            if (!_LookupVertex(to)) {  // to vertex not found:
                return false;
            }

            // create a new edge
            Edge* newEdge = new Edge();
            newEdge->edgeWeight = weight;
            newEdge->vertexId = to;

            // case for adding an edge the first time
            if (!this->adjList.at(from)){
                this->adjList.at(from) = newEdge;
            }
            else{

                Edge* currEdge = this->adjList.at(from);

                // loop through the linked list to find the appropriate place to add the new edge
                // and if the edge already exists, its weight is overridden
                while (currEdge->next){
                    
                    if (currEdge->vertexId == to){
                        currEdge->edgeWeight = weight;

                        delete newEdge;
                        return true;
                    }

                    currEdge = currEdge->next;
                }

                if (currEdge->vertexId == to){
                    currEdge->edgeWeight = weight;

                    delete newEdge;
                    return true;
                }

                currEdge->next = newEdge;

            }

            this->totEdges++;
            return true;
        }

        //
        // getWeight
        //
        // Returns the weight associated with a given edge.  If
        // the edge exists, the weight is returned via the reference
        // parameter and true is returned.  If the edge does not
        // exist, the weight parameter is unchanged and false is
        // returned.
        //
        bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
            
            // checks if both vertices exists in the graph
            if (!_LookupVertex(from)) {  
                return false;
            }

            if (!_LookupVertex(to)) { 
                return false;
            }

            // attempts to find the edge and change the weight parameter
            if (!this->adjList.at(from)){
                return false;
            }
            
            Edge* currEdge = this->adjList.at(from);

            while (currEdge){

                if (currEdge->vertexId == to){
                    weight = currEdge->edgeWeight;
                    return true;
                }

                currEdge = currEdge->next;
            }

            return false;
        }

        //
        // neighbors
        //
        // Returns a set containing the neighbors of v, i.e. all
        // vertices that can be reached from v along one edge.
        // Since a set is returned, the neighbors are returned in
        // sorted order; use foreach to iterate through the set.
        //
        set<VertexT> neighbors(VertexT v) const {
            set<VertexT>  S;

            if (!_LookupVertex(v)) {  // vertex not found
                return S;
            }

            Edge* currEdge = this->adjList.at(v);

            while (currEdge){
                S.insert(currEdge->vertexId);
                currEdge = currEdge->next;
            }

            return S;
        }

        //
        // getVertices
        //
        // Returns a vector containing all the vertices currently in
        // the graph.
        //
        vector<VertexT> getVertices() const {
            vector<VertexT> vertices;

            for (const auto& pair : this->adjList){
                vertices.push_back(pair.first);
            }

            return vertices;

        }

        //
        // dump
        //
        // Dumps the internal state of the graph for debugging purposes.
        //
        // Example:
        //    graph<string,int>  G(26);
        //    ...
        //    G.dump(cout);  // dump to console
        //
        void dump(ostream& output) const {
            output << "***************************************************" << endl;
            output << "********************* GRAPH ***********************" << endl;

            output << "**Num vertices: " << this->NumVertices() << endl;
            int i = 1;
            output << " **Vertices:" << endl;
            for (const auto& pair : this->adjList){
                output << "  " << i << ". " << pair.first << endl;
                i++;
            }
            output << endl;

            output << "**Num edges: " << this->NumEdges() << endl;
            output << " **Edges:" << endl;
            for (const auto& pair : this->adjList){
                output << "  " << pair.first << ": ";

                Edge* currEdge = pair.second;

                if (!currEdge){
                    output << "(None)\n";
                }
                else{

                    while (currEdge){
                        output << "(" << pair.first << "," << currEdge->vertexId << "," << currEdge->edgeWeight << ") ";
                        currEdge = currEdge->next;
                    }
                    output << endl;
                }

            }
            
            output << endl;
            

            output << "**************************************************" << endl;
        }
};
