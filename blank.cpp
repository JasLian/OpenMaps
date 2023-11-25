#include "graph.h"
#include <cstdlib>

using namespace std;

int main(){

    graph<char, int> test;
    for (int i = 0; i < 10; i++){
        test.addVertex('a' + i);
    }


    for (int i = 0; i < 100; i++){
        test.addEdge('a' + (rand() & 26), 'a' + (rand() % 26), i);
    }

    test.dump(cout);

    graph<char, int> copy;
    for (int i = 0; i < 20; i++){
        copy.addVertex('a' + i);
    }


    for (int i = 0; i < 100; i++){
        copy.addEdge('a' + (rand() & 26), 'a' + (rand() % 26), i);
    }
    
    cout << (copy.NumVertices() == test.NumVertices()) << endl;

    copy = test;
    cout << (copy.NumVertices() == test.NumVertices()) << endl;
}