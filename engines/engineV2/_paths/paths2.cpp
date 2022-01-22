
#include <emscripten.h>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <set>
#include <algorithm>
#include <queue>
#include <limits.h>
#include <math.h>

using namespace std;

struct context
{
    SDL_Renderer *renderer;
    int iteration;
};

//View
float WIDTH = 1920;
float HEIGHT = 1080;
float tilesize = 64;

//Mouse
bool mousedown = false;
int mousex, mousey;
int mouse_tilex;
int mouse_tiley;
int _x;
float alt;
bool clickblock = false;

//Paths
pair<float, float> start;
pair<float, float> endp;
bool start_set = false;
bool end_set = false;

int timet{};
bool pathset = false;
vector<pair<pair<float, float>, int>> finalpath;
bool dontlog;

vector<pair<int, int>> walls;
bool wallmode{false};
vector<int> paths;
vector<pair<int, int>> nodes;

float W = (WIDTH/tilesize);
float H = (HEIGHT/tilesize);

vector<pair<int, int>> neighbors{
    {1,0}, {0,1}, {-1,0}, {0,-1},
    {1,-1}, {-1,-1}, {1,1}, {-1,1}  
};

// typedef unordered_map<int, pair<int, int>> Graph;

inline int heuristic(pair<int, int> a, pair<int, int> b) {
  return abs(a.first - b.first) + abs(a.second - b.second);
}

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

vector<int> reconstruct_path(int start, int goal, unordered_map<int, int> came_from) {
    vector<int> path;
    int current = goal;
    while (current != start) {
        path.push_back(current);
        current = came_from[current];
    }
    //path.push_back(start); // optional
    //reverse(path.begin(), path.end());
    return path;
}

void dijkstra(Graph graph, int start, int goal, unordered_map<int, int>& came_from, unordered_map<int, int>& cost_so_far) {
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


int SDLCALL EventHandler(void *userdata, SDL_Event *event) {
    switch(event->type) {
        case SDL_MOUSEMOTION:
            mousex = event->motion.x;
            mousey = event->motion.y;             
            break;

        case SDL_MOUSEBUTTONDOWN:
            mousedown = true;
            alt = 1.3;
            break;   

        case SDL_MOUSEBUTTONUP:
            mousedown = false;
            clickblock = false;
            alt = 1;
            break;
        
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_w: wallmode = true; break;
                default:
                    break;
            }
            break;

        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_w: wallmode = false; break;
                default:
                    break;
            }
            break;
    }

    return -1;
}

bool initialized{false};
void mainloop(void *arg)
{   
    SDL_Event event;
    //Handle events
    while (SDL_PollEvent(&event)) {
        EventHandler(0, &event);
    }

    for(int i = 0; i < WIDTH/tilesize + 1; i++){
        _x = i*static_cast<int>(tilesize);
        
        if(mousex > _x){
            mouse_tilex = _x;
        }
    }
    for(int i = 0; i < HEIGHT/tilesize + 1; i++){
        _x = i*static_cast<int>(tilesize);
        
        if(mousey > _x){
            mouse_tiley = _x;
        }
    }

    if(!wallmode && mousedown && !clickblock) {
        if(!start_set) {
            start_set = true;
            start = {mouse_tilex, mouse_tiley};
        }
        else {
            if(mouse_tilex == start.first && mouse_tiley == start.second) {
                start_set = false; 
                end_set = false;
                pathset = false;
            }
            else {
                if(!end_set) {
                    end_set = true;
                    endp = {mouse_tilex, mouse_tiley};
                }
                else if(mouse_tilex == endp.first && mouse_tiley == endp.second) {
                    end_set = false; 
                    pathset = false;
                    dontlog = false;
                }
            }
        }
        clickblock = true;
    }
    

    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
    SDL_RenderClear(renderer);

    SDL_Rect temp_rect;
    //draw fake grid by overlapping black and white squares
    for(float x = 0; x < WIDTH/tilesize; ++x) {
        for(float y = 0; y < HEIGHT/tilesize; ++y) {

            temp_rect.w = tilesize;
            temp_rect.h = tilesize;
            temp_rect.x = (x*tilesize);
            temp_rect.y = (y*tilesize);

            if(x == start.first && y == start.second) 
                SDL_SetRenderDrawColor(renderer, 0, 222, 0, 255 );
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
            SDL_RenderFillRect(renderer, &temp_rect);

            temp_rect.w = tilesize - (tilesize/20);
            temp_rect.h = tilesize - (tilesize/20);
            temp_rect.x = (x*tilesize) + (tilesize/20);
            temp_rect.y = (y*tilesize) + (tilesize/20);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255 );
            SDL_RenderFillRect(renderer, &temp_rect);

        }
    }

    temp_rect.w = tilesize;
    temp_rect.h = tilesize;
    temp_rect.x = mouse_tilex;
    temp_rect.y = mouse_tiley;

    
    vector<pair<int, int>>::iterator it;
    int index{};
    //mouse node
    if(wallmode && mousedown) {
        SDL_SetRenderDrawColor(renderer, 221, 123, 12, 255 );
        SDL_RenderFillRect(renderer, &temp_rect);

        pair<int, int> wall{mouse_tilex, mouse_tiley};
        it = find(walls.begin(), walls.end(), wall);
        if(it != walls.end()) {
            index = distance(walls.begin(), it);
            walls.erase(walls.begin() + index);
        }
        else walls.push_back(wall);
        mousedown = false;
        
    }
    else {
        SDL_SetRenderDrawColor(renderer, 123*alt, 123*alt, 123*alt, 255 );
        SDL_RenderFillRect(renderer, &temp_rect);
    }

    if(!wallmode && start_set) {
        temp_rect.w = tilesize;
        temp_rect.h = tilesize;
        temp_rect.x = start.first;
        temp_rect.y = start.second;
        SDL_SetRenderDrawColor(renderer, 3, 222, 1, 255 );
        SDL_RenderFillRect(renderer, &temp_rect);

        if(end_set) {
            if(!pathset){
                pathset = true;
                paths.clear();

                index = 0;
                

                nodes.clear();

                int dest{-1};
                int src{-1};
                for(int h = 0; h < H; ++h) {
                    for(int w = 0; w < W; ++w) {
                        nodes.push_back({w, h});
                        if(w == static_cast<int>(endp.first/tilesize) && h == static_cast<int>(endp.second/tilesize)) dest = index;
                        if(w == static_cast<int>(start.first/tilesize) && h == static_cast<int>(start.second/tilesize)) src = index;
                        index++;
                    }
                }

                Graph graph2;
                vector<vector<int>> graph;
                vector<int> weights(W*H, -1);
                for(int i = 0; i < W*H; ++i) {
                    graph.push_back(weights);
                }
                
                bool isneigh{false};
                int neigh_index = -1;
                for(float i = 0; i < W*H; ++i) {

                    // for(float j = 0; j < W*H; ++j) {
                    //     graph[i][j] = 0;
                    // }
                    for(int n = 0; n < 8; ++n) {
                        
                        //check if the neighbor is within the bounds of the view
                        if(nodes[i].first + neighbors[n].first > -1 && nodes[i].second + neighbors[n].second > -1) {

                            //cout << i << " + " << neighbors[n] << "=" <<  i + neighbors[n] << " ";
                            pair<int, int> p{nodes[i].first + neighbors[n].first, nodes[i].second + neighbors[n].second};

                            it = find(nodes.begin(), nodes.end(), p);
                            if(it != nodes.end()) {
                                index = distance(nodes.begin(), it);
                                // if(abs(neighbors[n].first) == 1 && abs(neighbors[n].second) == 1) {
                                //     graph[i][index] = 2;
                                // }
                                // else graph[i][index] = 1;
                                graph2.edges[i].push_back(index);
                            }                            
                        }
                    }
                }

                unordered_map<int, int> came_from;
                unordered_map<int, int> cost_so_far;

                dijkstra(graph2, src, dest, came_from, cost_so_far);
                paths = reconstruct_path(src, dest, came_from);
                index = 0;
            }

            for(auto path : paths) {
                temp_rect.x = nodes[path].first*tilesize;
                temp_rect.y = nodes[path].second*tilesize;
                temp_rect.w = tilesize;
                temp_rect.h = tilesize;
                SDL_SetRenderDrawColor(renderer, 224, 222, 111, 255 );
                SDL_RenderFillRect(renderer, &temp_rect);
            }

            temp_rect.w = tilesize;
            temp_rect.h = tilesize;
            temp_rect.x = endp.first;
            temp_rect.y = endp.second;
            SDL_SetRenderDrawColor(renderer, 224, 2, 1, 255 );
            SDL_RenderFillRect(renderer, &temp_rect);

        }
        
    }

    for(auto wall : walls) {
        temp_rect.w = tilesize;
        temp_rect.h = tilesize;
        temp_rect.x = wall.first;
        temp_rect.y = wall.second;
        
        SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255 );
        SDL_RenderFillRect(renderer, &temp_rect);
    }
    //finally draw everything to the screen
    SDL_RenderPresent(renderer);
    ctx->iteration++;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)
    //emscripten_run_script("var ws = new WebSocket('wss://robauis.me/ws'); ws.onmessage = (e)=>{console.log(e.data);}");

    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
