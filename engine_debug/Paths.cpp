#include "Paths.h"
#include <stdio.h>
using namespace std;


vector<pair<int, int>> neighbors{
    {1,0}, {0,1}, {-1,0}, {0,-1},
    {1,-1}, {-1,-1}, {1,1}, {-1,1}  
};

inline int heuristic(pair<int, int> a, pair<int, int> b) {
  return abs(a.first - b.first) + abs(a.second - b.second);
}

vector<int> reconstruct_path(int start, int goal, unordered_map<int, int> came_from) {
    if(start == goal) return vector<int>{-1};
    vector<int> path;
    int current = goal;
    while (current != start) {
        path.push_back(current);
        current = came_from[current];
    }
    //path.push_back(start); // optional
    reverse(path.begin(), path.end());
    return path;
}

void dijkstra(Graph graph, vector<pair<int, int>> nodes, int start, int goal, unordered_map<int, int>& came_from, unordered_map<int, int>& cost_so_far) {
    PriorityQueue<int, int> frontier;
    frontier.put(start, 0);

    came_from[start] = 0;
    cost_so_far[start] = 0;

    while (!frontier.empty()) {
        int current = frontier.get();

        if (current == goal) {
            break;
        }

        for (int next : graph.neighbors(current)) {
            int new_cost = cost_so_far[current] + 1; //graph.cost(current, next);
            if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next]) {
                cost_so_far[next] = new_cost;
                int priority = new_cost + heuristic(nodes[next], nodes[goal]);
                frontier.put(next, priority);
                came_from[next] = current;
            }
        }
    }
}

pair<int, int> ConstructGraph(int tilesize, pair<int, int> origin, pair<int, int> src, int width, int height, pair<int, int> dest) {
    int index;
    int dest_index;
    int obj_index;
    Graph graph;
    vector<pair<int, int>> nodes;
    for(int w = origin.first; w < origin.first + width; ++w) {
        for(int h = origin.second; h < origin.second + height; ++h) {
            nodes.push_back({w, h});
        }
    }
    
    vector<pair<int, int>>::iterator it;
    it = find(nodes.begin(), nodes.end(), src);
    if(it != nodes.end()) {
        obj_index = distance(nodes.begin(), it);
    }
    it = find(nodes.begin(), nodes.end(), dest);
    if(it != nodes.end()) {
        dest_index = distance(nodes.begin(), it);
    }
    else return {-1,-1};

    for(float i = 0; i < (width)*(height); ++i) {
        for(int n = 0; n < 8; ++n) {

            pair<int, int> p{
                nodes[i].first + neighbors[n].first, 
                nodes[i].second + neighbors[n].second
            };

            it = find(nodes.begin(), nodes.end(), p);
            if(it != nodes.end()) {
                index = distance(nodes.begin(), it);
                graph.edges[i].push_back(index);
            }                            
        }
    }

    unordered_map<int, int> came_from;
    unordered_map<int, int> cost_so_far;
    dijkstra(graph, nodes, obj_index, dest_index, came_from, cost_so_far);
    int next_index = reconstruct_path(obj_index, dest_index, came_from)[0];
    if(next_index != -1)
        return nodes[next_index];
    else return {-1,-1};
}