
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

// A utility function to find the  
// vertex with minimum distance 
// value, from the set of vertices 
// not yet included in shortest 
// path tree 
int minDistance(int dist[],  
                bool sptSet[], int V) 
{ 
      
    // Initialize min value 
    int min = INT_MAX, min_index; 
  
    for (int v = 0; v < V; v++) 
        if (sptSet[v] == false && 
                   dist[v] <= min) 
            min = dist[v], min_index = v; 
  
    return min_index; 
} 
  
// Function to print shortest 
// path from source to j 
// using parent array 
void printPath(int parent[], int j, int V, vector<pair<int, int>> nodes, int dest, vector<pair<int, int>> & paths, int src) 
{ 
    pair<int, int> node{nodes[j].first, nodes[j].second};
    paths.push_back(node);
    //
    // Base Case : If j is source 
    if (parent[j] == src) 
        return; 
  
    printPath(parent, parent[j], V, nodes, dest, paths, src); 
  
    printf("(%d,%d)", nodes[j].first, nodes[j].second); 
    
    
    
} 
  
// A utility function to print  
// the constructed distance 
// array 
void printSolution(int dist[], int V,  
                      int parent[], vector<pair<int, int>> nodes, int dest, vector<pair<int, int>> & paths, int src) 
{ 
    //int src = 0; 
    //printf("Vertex\t Distance\tPath"); 
    bool found{false};
    for (int i = 0; i < V; i++) 
    { 
        //printf("\n%d -> %d \t\t %d\t\t%d ", 
        //              src, i, dist[i], src); 
        if(i == dest) {
            
            printPath(parent, i, V, nodes, dest, paths, src); 
            // for(int o = i; o > 0; --o) {
            //     printf("(%d,%d)", nodes[parent[o]].first, nodes[parent[o]].second); 
            // }
            //printf("_\n");
            found = true;
            break;
        }
        
    } 
    if(!found)
        cout << "no path found" << endl;
} 
  
// Funtion that implements Dijkstra's 
// single source shortest path 
// algorithm for a graph represented 
// using adjacency matrix representation 
void dijkstra(vector<vector<int>> graph, int src, int V,  
              vector<pair<int, int>> nodes, int dest, 
              vector<pair<int, int>> & paths) 
{ 
      
    // The output array. dist[i] 
    // will hold the shortest 
    // distance from src to i 
    int dist[V];  
  
    // sptSet[i] will true if vertex 
    // i is included / in shortest 
    // path tree or shortest distance  
    // from src to i is finalized 
    bool sptSet[V]; 
  
    // Parent array to store 
    // shortest path tree 
    int parent[V]; 
  
    // Initialize all distances as  
    // INFINITE and stpSet[] as false 
    for (int i = 0; i < V; i++) 
    { 
        parent[i] = -1; 
        dist[i] = INT_MAX; 
        sptSet[i] = false; 
    } 

    vector<pair<int, int>> neighbors{
        {1,0}, {1,1}, {0,1}, {-1,-1},
        {-1,0},{-1,-1}, {0,-1}, {1,-1}
        
    };
    
  
    // Distance of source vertex  
    // from itself is always 0 
    dist[src] = 0; 
    parent[src] = src;
    // Find shortest path 
    // for all vertices 
    for (int count = 0; count < V - 1; count++) 
    { 
        // Pick the minimum distance 
        // vertex from the set of 
        // vertices not yet processed.  
        // u is always equal to src 
        // in first iteration. 
        int u = minDistance(dist, sptSet, V); 
  
        // Mark the picked vertex  
        // as processed 
        sptSet[u] = true; 
  
        // Update dist value of the  
        // adjacent vertices of the 
        // picked vertex. 
        for (int v = 0; v < V; v++) {
            // Update dist[v] only if is 
            // not in sptSet, there is 
            // an edge from u to v, and  
            // total weight of path from 
            // src to v through u is smaller 
            // than current value of 
            // dist[v] 
            if (!sptSet[v] && graph[u][v] && dist[u] + graph[u][v] < dist[v]) { 
                parent[v] = u; 
                dist[v] = dist[u] + graph[u][v]; 
            }  
        } 
    }
    // print the constructed 
    // distance array 
    printSolution(dist, V, parent, nodes, dest, paths, src); 
} 

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
vector<pair<int, int>> paths;

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
        //Unnecessary re-initialization 
        _x = i*static_cast<int>(tilesize);
        
        if(mousex > _x){
            mouse_tilex = _x;
        }
    }
    for(int i = 0; i < HEIGHT/tilesize + 1; i++){
        //Unnecessary re-initialization 
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
                //  Let us create the example 
                // graph discussed above 
                index = 0;
                float W = (WIDTH/tilesize);
                float H = (HEIGHT/tilesize);

                // float neighbors[8] = {
                //     -W-1, 
                //     -W, 
                //     -W+1, 
                //     W+1, 
                //     W, 
                //     W-1,
                //     -1,
                //     1, 
                // };

                vector<pair<int, int>> neighbors{
                    {1,0}, {0,1}, {-1,0}, {0,-1},
                    {1,-1}, {-1,-1}, {1,1}, {-1,1}
                    
                };

                vector<pair<int, int>> nodes;
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

                //int graph[81][81];
                vector<vector<int>> graph;
                vector<int> weights(W*H, -1);
                for(int i = 0; i < W*H; ++i) {
                    graph.push_back(weights);
                }
                bool isneigh{false};
                int neigh_index = -1;
                for(float i = 0; i < W*H; ++i) {

                    for(float j = 0; j < W*H; ++j) {
                        graph[i][j] = 0;
                    }
                    for(int n = 0; n < 8; ++n) {

                        //if(i + neighbors[n] > -1) {
                        if(nodes[i].first + neighbors[n].first > -1 && nodes[i].second + neighbors[n].second > -1) {
                            //cout << i << " + " << neighbors[n] << "=" <<  i + neighbors[n] << " ";
                            pair<int, int> p{nodes[i].first + neighbors[n].first, nodes[i].second + neighbors[n].second};
                            pair<int, int> w{(nodes[i].first + neighbors[n].first)*tilesize,
                                             (nodes[i].second + neighbors[n].second)*tilesize};
                            it = find(walls.begin(), walls.end(), w);
                            if(it != walls.end()) {
                                cout << "YES" << endl;
                                graph[i][index] = INT_MAX;
                            }
                            else {
                                it = find(nodes.begin(), nodes.end(), p);
                                if(it != nodes.end()) {
                                    index = distance(nodes.begin(), it);
                                    if(abs(neighbors[n].first) == 1 && abs(neighbors[n].second) == 1) {
                                        graph[i][index] = 2;
                                    }
                                    else graph[i][index] = 1;
                                }
                            }
                            
                        }
                    }
                }

                index = 0;
                int index2{0};
                // for(int n = 0; n < W*H; ++n) {
                //     cout << n << ": ";
                //     for(int nn = 0; nn < W*H; ++nn) {
                //         if(graph[n][nn] == 1)
                //             cout << "(" << nodes[nn].first << "," << nodes[nn].second << ") ";
                //     }cout << endl;
                // }
            // printf("%d\n%d\n", src, dest);
                dijkstra(graph, src, W*H, nodes, dest, paths); 
            }
            for(auto path : paths) {
                temp_rect.w = tilesize;
                temp_rect.h = tilesize;
                temp_rect.x = path.first*tilesize;
                temp_rect.y = path.second*tilesize;
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
