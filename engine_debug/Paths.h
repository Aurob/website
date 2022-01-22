#include <unordered_map>
#include <vector>
#include <math.h>
#include <queue>

using namespace std;

inline int heuristic(pair<int, int>, pair<int, int>);

struct Graph {
    unordered_map<int, vector<int>> edges;

    vector<int> neighbors(int id) {
        return edges[id];
    }
};

template<typename T, typename priority_t>
struct PriorityQueue {
  typedef pair<priority_t, T> PQElement;
  priority_queue<PQElement, vector<PQElement>,
                 greater<PQElement>> elements;

  inline bool empty() const {
     return elements.empty();
  }

  inline void put(T item, priority_t priority) {
    elements.emplace(priority, item);
  }

  T get() {
    T best_item = elements.top().second;
    elements.pop();
    return best_item;
  }
};

vector<int> reconstruct_path(int, int, unordered_map<int, int>);
void dijkstra(Graph, vector<pair<int, int>>, int, int, unordered_map<int, int>&, unordered_map<int, int>&); 

pair<int, int> ConstructGraph(int, pair<int, int>,pair<int, int>, int, int, pair<int, int>);