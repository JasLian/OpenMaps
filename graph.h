// graph.h <Starter Code>
// < Your name >
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
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
    
        struct Edge{
            WeightT edgeWeight;
            VertexT vertexId;
            Edge* next = nullptr;
            
        };

        map<VertexT, Edge*> adjList;
        int totEdges = 0;

        //
        // _LookupVertex
        //
        int _LookupVertex(VertexT v) const {
            return adjList.count(v);
        }


    public:
        //
        // default constructor:
        //
        graph() {}

        graph(const graph& other){
            this->~graph();

            vector<VertexT> vertices = other.getVertices();
            for (VertexT vertex : vertices){
                this->addVertex(vertex);
            }

            for (VertexT vertex : vertices){

                set<VertexT> neighbors = other.neighbors(vertex);

                for (VertexT n : neighbors){
                    WeightT weight;
                    other.getWeight(vertex, n, weight);

                    this->addEdge(vertex, n, weight);
                }
            }
        }

        graph& operator=(const graph& other){
            this->~graph();

            graph<VertexT, WeightT> copy{other};
            return copy;
        }

        //
        // destructor:
        // frees all allocated memory of each Edge node
        //
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

            //
            // is the vertex already in the graph?  If so, we do not
            // insert again otherwise Vertices may fill with duplicates:
            //
            if (_LookupVertex(v) == 1) {
                return false;
            }

            this->adjList.emplace(v, nullptr);

            return true;
        }

        //
        // addEdge
        //
        // Adds the edge (from, to, weight) to the graph, and returns
        // true.  If the vertices do not exist, false is returned.
        //
        // NOTE: if the edge already exists, the existing edge weight
        // is overwritten with the new edge weight.
        //
        bool addEdge(VertexT from, VertexT to, WeightT weight) {
            
            if (!_LookupVertex(from)) {  // from vertex not found:
                return false;
            }

            if (!_LookupVertex(to)) {  // to vertex not found:
                return false;
            }

            Edge* newEdge = new Edge();
            newEdge->edgeWeight = weight;
            newEdge->vertexId = to;

            if (!this->adjList.at(from)){
                this->adjList.at(from) = newEdge;
            }
            else{

                Edge* currEdge = this->adjList.at(from);

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
            //
            // we need to search the Vertices and find the position
            // of each vertex; this will denote the row and col to
            // access in the adjacency matrix:
            //
            if (!_LookupVertex(from)) {  // from vertex not found:
                return false;
            }

            if (!_LookupVertex(to)) {  // to vertex not found:
                return false;
            }

            //
            // the vertices exist, but does the edge exist?
            //
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

            if (!_LookupVertex(v)) {  // vertex not found:
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
