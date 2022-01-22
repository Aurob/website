#pragma once
#include <vector>
#include <map>
#include "TextureUtils.h"
#include "WorldUtils.h"
#include "entt.h"
#include "FastNoise.h"
#include "Paths.h"
#include <SDL_thread.h>

#define WIDTH 1024
#define HEIGHT 768
using namespace std;

struct Tile {
    int x, y;
    int gx, gy;
    float n;
    int r, g, b;
    int ID;
};

struct Rendered {
    bool render;
};

struct Effect {
    unsigned int time;
    float x, y;
    int gx, gy;
    int size;
};

struct Button {
    int x, y;
    string action;
    int ID;
};

struct UIBlock {
    int x, y; //screen point
    int w, h;
    vector<pair<string, int>> content;
    int ID;
    int _w, _h;
    int ix, iy; //interaction point
    bool interacting;
    bool shown;
    int gx, gy; //global point
    bool pinned;
    int type; //0 interaction window, 1 button
    bool minimized;
};

struct Object {
    int x, y;
    int sx, sy;
    int type;
    int ID;
    int w, h;
    bool rendered;
    int gx, gy;
};

struct Building {
    int gx, gy;
    int x, y;
    int width, height;
    int doorx, doory;
    int ID;
    vector<Object> objects;
};

struct Movement {
    int gx, gy;
    int sx, sy;
    float speed;
};

struct Position {
    int gx, gy; //global
    int tx, ty; //tile
    int sx, sy; //screen
};

struct Tree {
    int w, h;
    int health;
    int loot;
};

struct User {
    float global[2];
    int temp_global[2];

    float inside_pos[2];
    float temp_inside[2];

    float tile[2];
    float mouse[2];
    int mouse_chunk[2];
    float tile_noise; 
    float speed;
    float speed_mod;
    int direction;
    bool idle;
    bool mousedown;
    unsigned int mousedown_time{};
    bool dead;
    int texindex;

    Building building;
    float health;
    bool inside;
    bool slowed;

    map<int, int> inventory;
    bool can_interact;
    int index;
};

class Game {
    public:
    const int default_tilesize = 64;
    int tilesize = 64;
    entt::registry registry;
    TextureUtils TU;
    WorldUtils WU;
    // FastNoise noise;
    map<SDL_Scancode, bool> key_states;
    bool explosion_click;

    int zoom_offsetx;
    int zoom_offsety;

    int view[4][2];
    int temp_view[4][2];
    int offset[2];

    User user;

    hash<string> hasher;

    vector<int> TileIDs;
    map<int, Tile> viewTiles;
    vector<Tile> visibleTiles;

    bool GUIshown;
    vector<string> info_list;
    vector<string> popup_list;

    vector<int> ObjIDs;
    string obj_types[5]{"stone", "plant", "animal", "food", "wood"};
    map<int, int> obj_spawn_timers;

    vector<Tile> clouds;
    int bird_count;

    float deltaTime;
    int lastTime;
    int dayTime;
    int tempTime;
    bool inside;
    
    float n;
    int r, g, b, rgb;
    int doorx, doory;
    int w, h;
    int gx, gy;
    int dx;
    int dy;
    int ID;
    int info_h;

    bool show_help;
    bool cancel_update;
    
    Game();
    void EventHandler(void*, SDL_Event*);
    void UpdateView();
    void UpdateUI();
    bool UpdateObjects();
    void UpdateEffects();
    
};
//void setLuminosity(float, int&, int&, int&);
int range(float,float,float,float,float);