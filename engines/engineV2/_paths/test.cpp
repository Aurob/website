// Program to find Dijkstra's shortest path using 
// priority_queue in STL 
#include<bits/stdc++.h> 
#include <algorithm>
#include <unordered_map>
#include <vector>
using namespace std; 
# define INF 0x3f3f3f3f 
  
// iPair ==>  Integer Pair 
typedef pair<int, int> iPair; 
  
// This class represents a directed graph using 
// adjacency list representation 
class Graph 
{ 
    int V;    // No. of vertices 
  
    // In a weighted graph, we need to store vertex 
    // and weight pair for every edge 
    list< pair<int, int> > *adj; 
  
public: 
    Graph(int V);  // Constructor 
  
    // function to add an edge to graph 
    void addEdge(int u, int v, int w); 
  
    // prints shortest path from s 
    void shortestPath(int s, vector<pair<int, int>>, int); 
}; 
  
// Allocates memory for adjacency list 
Graph::Graph(int V) 
{ 
    this->V = V; 
    adj = new list<iPair> [V]; 
} 
  
void Graph::addEdge(int u, int v, int w) 
{ 
    adj[u].push_back(make_pair(v, w)); 
    adj[v].push_back(make_pair(u, w)); 
} 

vector<pair<int, int>>::iterator it;
int pair_index(vector<pair<int, int>> vector, pair<int, int> value) {
    int index{-1};
    
    it = find(vector.begin(), vector.end(), value);
    if(it != vector.end()) {
        index = distance(vector.begin(), it);
    }

    return index;
}

// Driver program to test methods of graph class 
int main() 
{ 
    // create the graph given in above fugure 
    int V = 81; 
    Graph g(V); 
    vector<pair<int, int>> nodes;
    vector<vector<pair<int, int>>> graph;

    
    int source_node = 0;
    for(int w = 0; w < 9; ++w) {
        for(int h = 0; h < 9; ++h) {
            nodes.push_back({w, h});
        }
    }

    // for(auto ir = nodes.rbegin(); ir != nodes.rend(); ++ir) {
    //     nodes2.push_back(*ir);
    // }
    unordered_map<int, vector<int>> adjlist;
    int indexC{}, indexP{}, i2{};
    for(auto & c : nodes) {
        
        vector<pair<int, int>>::iterator it;
        it = find(nodes.begin(), nodes.end(), c);
        if(it != nodes.end()) {
            indexP = distance(nodes.begin(), it);
        }

        //get the index of each adjacent tile
        vector<pair<int, int>> neighbors{
            {c.first-1, c.second}, {c.first+1, c.second}, 
            {c.first-1, c.second-1}, {c.first+1, c.second-1},
            {c.first, c.second-1}, {c.first, c.second+1}, 
            {c.first-1, c.second+1}, {c.first+1, c.second+1}
        };


        for(auto n : neighbors) {
            it = find(nodes.begin(), nodes.end(), n);
            if(it != nodes.end()) {
                indexC = distance(nodes.begin(), it);
                //g.addEdge(indexP, indexC, 1); 
                adjlist[indexP].push_back(indexC);
            }
        }
    }

    priority_queue<int> working;
    working.push(0);

    vector<int> finished;
    vector<int>::iterator it;
    int END = 3;
    int node{-1};
    int index{};
    while(!working.empty()) {
        node = working.top();
        working.pop();
        //End reached
        if(node == END) break;
        
        //Check if node has been checked
        it = find(finished.begin(), finished.end(), node);
        if(it != finished.end()) {
            continue;
        }

        //add node to finished
        finished.push_back(node);

        for(int a : adjlist[node]) {
            //Check if node has been checked
            it = find(finished.begin(), finished.end(), a);
            if(it == finished.end()) {
                working.push(a);
            }
        }
    }
    

    cout << endl;
    for(auto node : finished) {
        cout << nodes[node].first << "," << nodes[node].second << endl;
    }

    // for(auto node : nodes) {
    //     cout << node.first << "," << node.second << endl;
    // }
    //g.shortestPath( pair_index(nodes, {0,0}), nodes, pair_index(nodes, {5,5})); 

    return 0; 
}