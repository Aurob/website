
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
pair<int, int> start;
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

vector<pair<int, int>>::iterator it;
unordered_map<int, int> came_from;
vector<pair<int, int>> nodes;
vector<int> path;

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

    if(wallmode && mousedown) {
        SDL_SetRenderDrawColor(renderer, 221, 123, 12, 255 );
        SDL_RenderFillRect(renderer, &temp_rect);

        pair<int, int> wall{mouse_tilex, mouse_tiley};
        it = find(walls.begin(), walls.end(), wall);
        if(it != walls.end()) {
            walls.erase(walls.begin() + distance(walls.begin(), it));
        }
        else walls.push_back(wall);
        mousedown = false;
        
    }
    else {
        SDL_SetRenderDrawColor(renderer, 123*alt, 123*alt, 123*alt, 255 );
        SDL_RenderFillRect(renderer, &temp_rect);
    }

    if(!wallmode && start_set) {
        if(end_set) {            
            if(!pathset){
                came_from.clear();
                nodes.clear();
                pathset = true;
                //set path
                float W = (WIDTH/tilesize);
                float H = (HEIGHT/tilesize);
                
                int dest{-1};
                int src{-1};
                int index = 0;
                for(int h = 0; h < H; ++h) {
                    for(int w = 0; w < W; ++w) {
                        nodes.push_back({w, h});
                        if(w == static_cast<int>(endp.first/tilesize) && h == static_cast<int>(endp.second/tilesize)) dest = index;
                        if(w == static_cast<int>(start.first/tilesize) && h == static_cast<int>(start.second/tilesize)) src = index;
                        index++;
                    }
                }

                unordered_map<int, vector<int>> neighbors;
                vector<pair<int, int>> adj{
                    {1,0}, {0,1}, {-1,0}, {0,-1},
                    //{1,-1}, {-1,-1}, {1,1}, {-1,1}  
                };
                for(int i = 0; i < W*H; ++i) {
                    for(auto n : adj) {
                        //if adjacent node is valid
                        if(n.first + nodes[i].first > -1 && n.second + nodes[i].second > -1) {
                            //find node in ndoelist
                            pair<int, int> neighbor{n.first + nodes[i].first, n.second + nodes[i].second};
                            it = find(nodes.begin(), nodes.end(), neighbor);
                            //if adjacent node is a valid node
                            if(it != nodes.end()) {
                                //add the node to the parent node
                                int nindex = distance(nodes.begin(), it);
                                pair<int, int> wall_index{nodes[nindex].first*tilesize, nodes[nindex].second*tilesize};
                                it = find(walls.begin(), walls.end(), wall_index);
                                if(it == walls.end()) {
                                    neighbors[i].push_back(nindex);
                                }                            
                            }
                        }
                        
                    };
                }
                
                queue<int> frontier;
                int start_node_index{-1};
                
                pair<int, int> s{start.first/tilesize, start.second/tilesize};
                it = find(nodes.begin(), nodes.end(), s);
                if(it != nodes.end()) {
                    start_node_index = distance(nodes.begin(), it);
                }
                if(start_node_index > -1){
                    frontier.push(start_node_index);
                    came_from[start_node_index] = -1;//start_node_index; //might bonk

                    int current{};
                    while(!frontier.empty()) {
                        current = frontier.front();
                        frontier.pop();
                        for(auto neigh : neighbors[current]) {
                            
                            unordered_map<int, int>::iterator umit;
                            umit = came_from.find(neigh);
                            // == : not found
                            if(umit == came_from.end()) {
                                
                                frontier.push(neigh);
                                came_from[neigh] = current;
                            }
                        }
                    }
                    
                    bool dest_found{};

                    path.clear();
                    pair<int, int> e{endp.first/tilesize, endp.second/tilesize};
                    it = find(nodes.begin(), nodes.end(), e);
                    if(it != nodes.end()) {
                        current = distance(nodes.begin(), it);

                        bool valid{false};
                        for(auto neigh : neighbors[current]) {
                            printf("%d %d %d\n", current, came_from[current], neigh);
                            if(came_from[current] == neigh) valid = true;
                        }
                        if(!valid) {
                            printf("Wont work\n");
                            return;
                        }
                        while(current != start_node_index) {
                            path.push_back(current);
                            int temp = current;
                            current = came_from[current];
                            // if(current == temp) {
                        }                   
                    }
                }
                
            }
            for(auto node : path) {
                temp_rect.w = tilesize;
                temp_rect.h = tilesize;
                temp_rect.x = nodes[node].first*tilesize;
                temp_rect.y = nodes[node].second*tilesize;
                SDL_SetRenderDrawColor(renderer, 184, 184, 184, 255 );
                SDL_RenderFillRect(renderer, &temp_rect);
            }
            temp_rect.w = tilesize;
            temp_rect.h = tilesize;
            temp_rect.x = endp.first;
            temp_rect.y = endp.second;
            SDL_SetRenderDrawColor(renderer, 224, 2, 1, 255 );
            SDL_RenderFillRect(renderer, &temp_rect);
        }
        temp_rect.w = tilesize;
        temp_rect.h = tilesize;
        temp_rect.x = start.first;
        temp_rect.y = start.second;
        SDL_SetRenderDrawColor(renderer, 3, 222, 1, 255 );
        SDL_RenderFillRect(renderer, &temp_rect);
    }

    for(auto wall : walls) {
        temp_rect.w = tilesize;
        temp_rect.h = tilesize;
        temp_rect.x = wall.first;
        temp_rect.y = wall.second;
        
        SDL_SetRenderDrawColor(renderer,0, 0, 0, 255 );
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
