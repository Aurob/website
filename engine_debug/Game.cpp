#include "Game.h"

// void update() {
//     printf("Hello\n");
// }
// int main() {
//     int gx, gy;
//     int bx, by;
//     for(int b = 0; b < 500; ++b) {
//         auto building = registry.create();
//         gx = ((rand()%2==0)?-1:1)*(rand()%1000);
//         gy = ((rand()%2==0)?-1:1)*(rand()%1000);
//         if(WU.terrainGeneration(gx, gy)>.45) {
//             bx = 5 + rand()%25;
//             by = 5 + rand()%25;
//             vector<Object> objects;
//             for(int i = 0; i < 10; ++i) {
//                 objects.push_back(Object{rand()%bx, rand()%by, 0, 0, 3});
//             }
//             registry.emplace<Building>(building, gx, gy, 0, 0, bx, by, (bx)/2, (by)-1, static_cast<int>(hasher(to_string(bx)+","+to_string(by))), objects);
//         }
//     }
//     user.global[0] = 10000;
//     user.global[1] = 10000;
// }

Game::Game() {
    dayTime = SDL_GetTicks();
    printf("Hello!\n");
    user.global[0] = 1000;
    user.global[1] = 1000;
    user.speed = default_tilesize*4;
    //ensure the user doesn't spawn in water
    // while(WU.terrainGeneration(user.global[0]/tilesize, user.global[1]/tilesize) < 0.45) {
    //     user.global[0] = 1000+rand()%9000;
    //     user.global[1] = 1000+rand()%9000;
    // }

    UIBlock help_popup{tilesize, tilesize, ((WIDTH/tilesize)-4)*tilesize, ((HEIGHT/tilesize)-4)*tilesize,vector<pair<string, int>>{
            {"Welcome To {UNNAMED_GAME}!", 1}, 
            {"Controls: ", 2},
            {"- Move Up : W", 4},
            {"- Move Down : S", 4},
            {"- Move Left : A", 4},
            {"- Move Right : D", 4},
            {"Interactions:", 2},
            {"- Click on the base of any tree within 2 tiles of your position", 1},
            {"- Click and hold the blue bar to move this window", 1}, 
            {"- Click the red box to minimize this window", 1}, 
        }
    };
    
    help_popup.type = 2;
    // help_popup.gx = ((user.global[0]/tilesize) - WIDTH/2)/tilesize;
    // help_popup.gy = ((user.global[1]/tilesize) - HEIGHT/2)/tilesize;
    //help_popup.pinned = true;
    help_popup.shown = true;
    const auto popup = registry.create();
    registry.emplace<UIBlock>(
        popup, 
        help_popup
    );

    UIBlock menu_button{145, 0, 70, 20, vector<pair<string, int>>{
            {"Menu", 1}
        }
    };
    menu_button.shown = true;
    const auto menu = registry.create();
    registry.emplace<UIBlock>(
        menu, 
        menu_button
    );

    const auto npc_test = registry.create();
    Object npc{
        10, 10, 0, 0, 5
    };
    npc.rendered = true;
    npc.gx = 10*tilesize;
    npc.gy = 10*tilesize;
    registry.emplace<Object>(npc_test, npc);
    registry.emplace<Movement>(npc_test);


    auto user = registry.create();
}

void Game::UpdateView() {

    //calculate the fps from time elapsed since the last frame
    deltaTime = (SDL_GetTicks() - lastTime);
    lastTime = SDL_GetTicks();
    dayTime += deltaTime;
    //use speed mod to reflect the fps
    user.speed_mod = (deltaTime/1000)*(user.speed - ((user.slowed)?(user.speed/1.2):0));

    //game time represents the time since it was last set
    //after n seconds, reset it 
    if(dayTime >= 60000) {
        dayTime = 0;
    } 
    
    //Allow user interaction only if the user's mouse is close enough to the user
    if(user.mouse[0] < (WIDTH/2) + tilesize*2 && user.mouse[0] > (WIDTH/2) - tilesize*2) {
        if(user.mouse[1] < (HEIGHT/2) + tilesize*2 && user.mouse[1] > (HEIGHT/2) - tilesize*2) {
            user.can_interact = true;
        } else user.can_interact = false;
    } else user.can_interact = false;

    //Separate rendering for inside vs outside
    if(!user.inside) {
        //Store the current position in case the update needs to be cancelled
        if(!cancel_update) {
            user.temp_global[0] = user.global[0];
            user.temp_global[1] = user.global[1];
        
            //Do event triggered updates
            if(key_states[SDL_SCANCODE_SPACE]) printf("Hello!\n");
            if(key_states[SDL_SCANCODE_W]) {
                user.global[1] -= user.speed_mod;//tilesize/4;
            }
            if(key_states[SDL_SCANCODE_A]) {
                user.global[0] -= user.speed_mod;
            }
            if(key_states[SDL_SCANCODE_S]) {
                user.global[1] += user.speed_mod;
            }
            if(key_states[SDL_SCANCODE_D]) {
                user.global[0] += user.speed_mod;
            }
        }

        //Simple update when the tilesize is the default size
        if(tilesize == default_tilesize){
            offset[0] = (fmod(user.global[0], static_cast<float>(tilesize)));
            offset[1] = (fmod(user.global[1], static_cast<float>(tilesize)));
            zoom_offsetx = 0;
            zoom_offsety = 0;
        }
        //If the tilesize is not default, add "zoom offsets" to ensure the view remains centered
        else{
            offset[0] = (fmod(user.global[0], static_cast<float>(default_tilesize)) / static_cast<float>(default_tilesize)) * static_cast<float>(tilesize);
            offset[1] = (fmod(user.global[1], static_cast<float>(default_tilesize)) / static_cast<float>(default_tilesize)) * static_cast<float>(tilesize);
            zoom_offsetx = (user.global[0] / static_cast<float>(default_tilesize)) - (user.global[0] / static_cast<float>(tilesize));
            zoom_offsety = (user.global[1] / static_cast<float>(default_tilesize)) - (user.global[1] / static_cast<float>(tilesize));
        }

        //Set the user's global tile
        user.tile[0] = static_cast<int>((((user.global[0])/default_tilesize)));
        user.tile[1] = static_cast<int>((((user.global[1])/default_tilesize)));

        //Get the terrain type of the user's tile
        user.tile_noise = WU.terrainGeneration(user.tile[0], user.tile[1]);
        //If the terrain is water, slow the user's speed
        if(user.tile_noise > 0.45)
            if(user.slowed) user.slowed = false;
        if(user.tile_noise < 0.45) user.slowed = true;

        
        //Set view bounds based on the user's global position
        //These values are used for determining which tiles to render
        view[0][0] = floor(user.tile[0] - ((WIDTH/tilesize)/2));
        view[0][1] = floor((user.tile[1] - ((HEIGHT/tilesize)/2));
        view[1][0] = view[0][0] + floor(WIDTH / tilesize);
        view[1][1] = view[0][1];
        view[2][0] = view[0][0];
        view[2][1] = view[0][1] + floor(HEIGHT / tilesize);
        view[3][0] = view[1][0];
        view[3][1] = view[2][1];

        //user.mouse_chunk[0] = (floor(user.mouse[0]]/tilesize)*tilesize)-offset[0];
        //user.mouse_chunk[1] = (floor(user.mouse[1]/tilesize)*tilesize)-offset[1];

        //TODO
        //resolve issues with negative coordinates
        dx = (offset[0] < 0) ? tilesize + offset[0] : offset[0];
        dy = (offset[1] < 0) ? tilesize + offset[1] : offset[1];

        //Update objects with new view data
        if(!cancel_update) {
            //UpdateObjects returns false if a collision with the user and an object occurs 
            if(!UpdateObjects()) {
                //Set the cancel flag to true, revert the global position and re-call UpdateView
                cancel_update = true;
                user.global[0] = user.temp_global[0];
                user.global[1] = user.temp_global[1];
                //Resets the view, returns early
                UpdateView();
                cancel_update = false;
            }
        }
        //A collision occured, cancel flag is true
        else {
            //Reset objects and return early
            UpdateObjects();
            return;
        }

        //Clear the visible tiles vector and calculate the current visible tiles
        visibleTiles.clear();
        for(int x = view[0][0]; x < view[1][0] + 1; ++x) {
            for(int y = view[0][1]; y < view[3][1] + 2; ++y) {

                //Generate the unique ID for this tile
                ID = hasher(to_string(x)+","+to_string(y));
                w = (-dx + ((x-view[0][0])*tilesize));
                h = (-dy + ((y-view[0][1])*tilesize));

                //If the tile ID is found in the viewTiles vector, then it has already been visited
                //Update the existing tile with new view data
                if(viewTiles.find(ID) != viewTiles.end()) {
                    //Tile tile{w, h, x, y, n, r, g, b, ID};
                    viewTiles[ID].x = w;
                    viewTiles[ID].y = h;
                    visibleTiles.push_back(viewTiles[ID]);
                }

                //Create a new tile
                else {
                    //const auto entity = registry.create();
                    srand(ID);
                    n = WU.terrainGeneration(x, y);
                    rgb = 256 * n;
                    if(n < 0.45){
                        r = 0; g = 123; b = 238; //water
                    }
                    else if(n < 0.49){ //sand
                        r = rgb*1.8; g = rgb*1.7; b = rgb*1.2; 
                    } 
                    else if(n < 0.62){ //grass
                        r = 139*n*1.2; g = 214*n*1.8; b = 74*1.5; 
                        
                        //Add default tree
                        if(rand() % 400 < 2) {
                            const auto tree = registry.create();
                            registry.emplace<Object>(tree, x, y, w, h, 4, ID, 2+rand()%5, 2+rand()%5);
                        }

                        if(rand() % 10000 < 100) {
                            const auto building = registry.create();
                            registry.emplace<Object>(building, x, y, w, h, 4, ID, 10, 10);
                        }
                    }
                    else { //stone
                        r = 139*(n*1.6); g = 139*(n*1.6); b = 139*(n*1.6);
                    }

                    //setLuminosity(50 + ((lastTime/100)%50), r, g, b);
                    Tile tile{w, h, x, y, n, r, g, b, ID};
                    viewTiles[ID] = tile;
                    visibleTiles.push_back(tile);
                }
            }
        }
    }
}

void Game::UpdateEffects() {
    auto effects = registry.view<Effect>();
    for(auto entity: effects) {
        auto &effect = effects.get<Effect>(entity);
        effect.x = (-dx + ((effect.gx-view[0][0])*tilesize));
        effect.y = (-dy + ((effect.gy-view[0][1])*tilesize));
    }
}

bool Game::UpdateObjects() {
    auto objects = registry.view<Object>();
    for(auto entity: objects) {
        auto &object = objects.get<Object>(entity);
        object.rendered = false;
        //registry.remove_if_exists<Rendered>(entity);

        //The object is within the view bounds
        if(object.x + object.w/2 >= view[0][0] && object.x - object.w/2 <= view[1][0]) {
            if(object.y >= view[0][1] && object.y - object.h <= view[2][1]) {
                //registry.emplace<Rendered>(entity);
                object.rendered = true;
                
                if(user.can_interact) {
                    if(user.mouse[0] < object.sx + tilesize && user.mouse[0] > object.sx) {
                        if(user.mouse[1] < object.sy + tilesize && user.mouse[1] > object.sy) {
                            if(user.mousedown) {
                                
                                //Add some sparkle effects around the user's cursor
                                const auto sparkle = registry.create();
                                registry.emplace<Effect>(
                                    sparkle, 
                                    SDL_GetTicks(), 
                                    user.mouse[0], user.mouse[1], 
                                    object.x, object.y,
                                    tilesize/2
                                );
                                
                                user.inventory[object.type]++;
                                registry.destroy(entity);
                                continue;
                            }
                        }
                    }
                }

                if(registry.has<Movement>(entity)) {
                    // if(object.x != user.tile[0] && object.y != user.tile[1]) {
                        pair<int, int> next_tile = 
                            ConstructGraph(tilesize, 
                                pair<int, int>{object.x - 10, object.y - 10}, 
                                pair<int, int>{object.x, object.y}, 
                                20, 20, 
                                pair<int, int>{user.tile[0], user.tile[1]});

                        if(next_tile.first != -1) {
                            if(next_tile.first > object.x) object.gx+=tilesize/10;
                            if(next_tile.first < object.x) object.gx-=tilesize/10;
                            if(next_tile.second > object.y) object.gy+=tilesize/10;
                            if(next_tile.second < object.y) object.gy-=tilesize/10;

                            object.x = floor(object.gx/default_tilesize);
                            object.y = floor(object.gy/default_tilesize);
                        }
                    // }

                    object.sx = (-dx + ((object.x-view[0][0])*tilesize));
                    object.sy = (-dy + ((object.y-view[0][1])*tilesize));
                    
                }
                else {
                    //cancel this render if the user walks into a building wall
                    if(user.tile[0] >= object.x && user.tile[0] <= object.x) {
                        if(user.tile[1] >= object.y && user.tile[1] <= object.y) {
                            return false;
                        }
                    }
                    object.sx = (-dx + ((object.x-view[0][0])*tilesize));
                    object.sy = (-dy + ((object.y-view[0][1])*tilesize));
                }

                
                
                
            }
        }
    }
    return true;
}

void Game::UpdateUI() {
    info_list.clear();
    info_list.push_back((GUIshown) ? "Hide Info" : "Show Info");
    if(user.mouse[0] < 140 && user.mouse[0] > 0) {
        if(user.mouse[1] < 20 && user.mouse[1] > 0) {
            if(user.mousedown) {
                GUIshown = !(GUIshown);
                user.mousedown = false;
            }
        }
    }

    if(user.mouse[0] < 210 && user.mouse[0] > 140) {
        if(user.mouse[1] < 20 && user.mouse[1] > 0) {
            if(user.mousedown) {
                user.mousedown = false;
                show_help = !(show_help);
            }
        }
    }
    if(GUIshown) {
        if(user.inside) {
            info_list.push_back("Inside | X: " +to_string(static_cast<int>(user.inside_pos[0]))+" Y: "+to_string(static_cast<int>(user.inside_pos[1])));
            info_list.push_back("Building ID: "+to_string(user.building.ID));

        }
        
        info_list.push_back("FPS: " + to_string((1000/deltaTime)));
        info_list.push_back("Time: " + to_string(dayTime));
        info_list.push_back("Global | X: " +to_string(static_cast<int>(user.global[0]))+" Y: "+to_string(static_cast<int>(user.global[1])));

        info_list.push_back("Inventory:");
        for(auto obj : user.inventory) {
            info_list.push_back("Type: "+obj_types[obj.first] + " ("+to_string(obj.second)+")");
        }
    }

    auto popups = registry.view<UIBlock>();
    for(auto entity: popups) {
        auto &popup = popups.get<UIBlock>(entity);
        
        //Popup ID#1234 is the Help window
        if(popup.ID == 1234) {
            if(show_help) popup.shown = true;
        }
        if(popup.pinned) {
            dx = (offset[0] < 0) ? tilesize + offset[0] : offset[0];
            dy = (offset[1] < 0) ? tilesize + offset[1] : offset[1];
            popup.x = (-dx + ((popup.gx-view[0][0])*tilesize));
            popup.y = (-dy + ((popup.gy-view[0][1])*tilesize));
        }
        if(!popup.interacting) {
            //Window interaction
            if(user.mouse[0] < popup.x + popup.w && user.mouse[0] > popup.x) {
                if(user.mouse[1] < popup.y+(popup.h/16) && user.mouse[1] > popup.y) {
                    if(user.mousedown) {
                        popup.ix = user.mouse[0] - popup.x;
                        popup.iy = user.mouse[1] - popup.y;
                        popup.pinned = false;
                        popup.interacting = true;
                        //continue;
                    }
                }
            }
        
            if(user.mouse[0] < (popup.x+(popup.w-(popup.w/16)))+(popup.w/16) && user.mouse[0] > popup.x+(popup.w-(popup.w/16))) {
                //Close interaction
                if(user.mouse[1] < popup.y+(popup.h/8) && user.mouse[1] > popup.y+popup.h/16) {
                    if(user.mousedown) {
                        show_help = false;
                        popup.shown = false;
                        popup.interacting = false;
                        //continue;
                    }
                }
                //Pin interaction
                else if(user.mouse[1] < popup.y+(popup.h/4) && user.mouse[1] > popup.y+(popup.h/8)) {
                    if(user.mousedown) {
                        user.mousedown = false;

                        if(popup.type == 1) {
                            //Pinnable window
                            if(!popup.pinned) {
                                popup.gx = view[0][0] + floor(popup.x/tilesize);
                                popup.gy = view[0][1] + floor(popup.y/tilesize);
                                popup.pinned = true;
                                popup.interacting = false;
                            }
                            else popup.pinned = false;
                        }
                        //continue;
                    }
                }
            }
        }
        //User is dragging window
        else {
            popup.x = user.mouse[0] - popup.ix;
            popup.y = user.mouse[1] - popup.iy;
            if(!user.mousedown){
                popup.interacting = false;
                //continue;
            }
        }
    }
}

void SDLCALL Game::EventHandler(void *userdata, SDL_Event *event) {
    switch(event->type) {
        case SDL_MOUSEBUTTONDOWN:
            user.mousedown = true;
            user.mousedown_time = SDL_GetTicks() - user.mousedown_time;
            break;   

        case SDL_MOUSEBUTTONUP:
            user.mousedown = false;
            user.mousedown_time = 0;
            break;

        case SDL_MOUSEMOTION:
            user.mouse[0] = event->motion.x;
            user.mouse[1] = event->motion.y;
            // printf("Pos: %d, %d\n", user.mouse[0], user.mouse[1]);
            break;

        case SDL_KEYUP:
            key_states[event->key.keysym.scancode] = false;
            break;

        case SDL_KEYDOWN:
            key_states[event->key.keysym.scancode] = true;
            break;
        case SDL_MOUSEWHEEL:
            if(event->wheel.y < 0 && tilesize > 2) {
                tilesize/=2;
                zoom_offsetx = (user.global[0]/default_tilesize) - (user.global[0]/tilesize);
                zoom_offsety = (user.global[1]/default_tilesize) - (user.global[1]/tilesize);
            }
            if(event->wheel.y > 0 && tilesize < 256) {
                tilesize*=2;
                zoom_offsetx = (user.global[0]/default_tilesize) - (user.global[0]/tilesize);
                zoom_offsety = (user.global[1]/default_tilesize) - (user.global[1]/tilesize);
            }
            break;
        default: break;
    }
}

//Adjusts the brightness of the color passed
// void Game::setLuminosity(float l, int& r, int& g, int& b) {
// }

int range(float n, float nmin, float nmax, float rmin, float rmax){
    return static_cast<int>(((n - nmin)/(nmax - nmin))*((rmax - rmin)+rmin));
}
