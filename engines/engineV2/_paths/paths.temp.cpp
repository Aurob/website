
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
void printPath(int parent[], int j, int V, vector<pair<int, int>> nodes, int dest) 
{ 
      
    // Base Case : If j is source 
    if (parent[j] == - 1) 
        return; 
  
    printPath(parent, parent[j], V, nodes, dest); 
  
    printf("(%d,%d)", nodes[j].first, nodes[j].second); 
} 
  
// A utility function to print  
// the constructed distance 
// array 
void printSolution(int dist[], int V,  
                      int parent[], vector<pair<int, int>> nodes, int dest) 
{ 
    int src = 0; 
    printf("Vertex\t Distance\tPath"); 
    for (int i = 1; i < V; i++) 
    { 
        if(i == dest) {
            printf("\n%d -> %d \t\t %d\t\t%d ", 
                      src, i, dist[i], src); 
            printPath(parent, i, V, nodes, dest); 
        }
        printf("\n");
        
    } 
} 
  
// Funtion that implements Dijkstra's 
// single source shortest path 
// algorithm for a graph represented 
// using adjacency matrix representation 
void dijkstra(vector<vector<int>> graph, int src, int V,  vector<pair<int, int>> nodes, int dest) 
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
        parent[0] = -1; 
        dist[i] = INT_MAX; 
        sptSet[i] = false; 
    } 
  
    // Distance of source vertex  
    // from itself is always 0 
    dist[src] = 0; 
  
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
        for (int v = 0; v < V; v++) 
  
            // Update dist[v] only if is 
            // not in sptSet, there is 
            // an edge from u to v, and  
            // total weight of path from 
            // src to v through u is smaller 
            // than current value of 
            // dist[v] 
            if (!sptSet[v] && graph[u][v] && 
                dist[u] + graph[u][v] < dist[v]) 
            { 
                parent[v] = u; 
                dist[v] = dist[u] + graph[u][v]; 
            }  
    } 
  
    // print the constructed 
    // distance array 
    printSolution(dist, V, parent, nodes, dest); 
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

    if(mousedown && !clickblock) {
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

    //mouse node
    SDL_SetRenderDrawColor(renderer, 123*alt, 123*alt, 123*alt, 255 );
    SDL_RenderFillRect(renderer, &temp_rect);

    if(start_set) {
        temp_rect.w = tilesize;
        temp_rect.h = tilesize;
        temp_rect.x = start.first;
        temp_rect.y = start.second;
        SDL_SetRenderDrawColor(renderer, 3, 222, 1, 255 );
        SDL_RenderFillRect(renderer, &temp_rect);

        if(end_set) {
            temp_rect.w = tilesize;
            temp_rect.h = tilesize;
            temp_rect.x = endp.first;
            temp_rect.y = endp.second;
            SDL_SetRenderDrawColor(renderer, 224, 2, 1, 255 );
            SDL_RenderFillRect(renderer, &temp_rect);

            //  Let us create the example 
            // graph discussed above 
            int index{};
            float V = (WIDTH/tilesize);

            float neighbors[8] = {
                1, V+1, V, V-1, -1, -V-1, -V, -V+1
            };

            vector<pair<int, int>> nodes;
            int dest{-1};
            int src{-1};
            for(int h = 0; h < V; ++h) {
                for(int w = 0; w < V; ++w) {
                    nodes.push_back({w, h});
                    if(w == static_cast<float>(endp.first/tilesize) && h == static_cast<float>(endp.second/tilesize)) dest = index;
                    else if(w == static_cast<float>(start.first/tilesize) && h == static_cast<float>(start.second/tilesize)) src = index;
                    index++;
                }
            }

            //int graph[81][81];
            vector<vector<int>> graph;
            vector<int> weights(V*V, -1);
            for(int i = 0; i < V*V; ++i) {
                graph.push_back(weights);
            }
            bool isneigh{false};
            int neigh_index = -1;
            for(float i = 0; i < V*V; ++i) {
                for(float j = 0; j < V*V; ++j) {
                    for(int n = 0; n < 8; ++n) {

                        if(i + neighbors[n] > -1) {
                            if(j == i + neighbors[n]) {
                                isneigh = true;
                            }
                        }
                    }
                    if(isneigh) { 
                        graph[i][j] = 1;
                        isneigh = false;
                    }
                    else graph[i][j] = 0;
                }
            }

            printf("%d\n", dest);
            dijkstra(graph, 0, V*V, nodes, dest); 
        }
    }

    if(start_set && end_set) {
         
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
