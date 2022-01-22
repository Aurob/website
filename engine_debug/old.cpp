//Get the rendering context
    deltaTime = (SDL_GetTicks() - lastTime);
    lastTime = SDL_GetTicks();

// if(user.mousedown) {
    //     int time_size=(SDL_GetTicks()-user.mousedown_time)/10;
    //     // const auto sparkle = registry.create();
    //     // registry.emplace<Effect>(
    //     //     sparkle, 
    //     //     SDL_GetTicks(), 
    //     //     user.mouse[0], user.mouse[1], 
    //     //     static_cast<unsigned int>((time_size>WIDTH/4?tilesize:time_size))
    //     // );
    
    // }
    // //Draw Effects
    // //Starting with sparkles
    // for(auto [entity, sparkle]: sparkles.each()) {
    //     if(SDL_GetTicks() - sparkle.time > 900) {
    //         registry.destroy(entity);
    //         continue;
    //     }

    //     if(explosion_click) {
    //         tex_rect.x = 96 * (sparkle.time/100%12); tex_rect.y = 0;
    //         tex_rect.w = 96; tex_rect.h = 96;
    //         screen_rect.x = sparkle.x-((sparkle.size*tilesize)/2); screen_rect.y = sparkle.y-((sparkle.size*tilesize)/2);
    //         screen_rect.w = sparkle.size*tilesize; screen_rect.h = sparkle.size*tilesize;
    //         SDL_RenderCopy(renderer, TU.Textures["boom"][0].tex, &tex_rect, &screen_rect); //static_cast<int>(sparkle.time/6)%4
    //     }
    //     else {
    //         tex_rect.x = 0; tex_rect.y = 0;
    //         tex_rect.w = 32; tex_rect.h = 32;
    //         screen_rect.x = sparkle.x-(sparkle.size/2); screen_rect.y = sparkle.y-(sparkle.size/2);
    //         screen_rect.w = sparkle.size; screen_rect.h = sparkle.size;
    //         SDL_RenderCopy(renderer, TU.Textures["food"][static_cast<int>(sparkle.time/6)%5].tex, &tex_rect, &screen_rect); //static_cast<int>(sparkle.time/6)%4
    //     }

        
    // }
    
float n;
    int r, g, b, rgb;
    int doorx, doory;
    int w, h;
    int gx, gy;
    int dx;
    int dy;
    int ID;
    int info_h;
    if(!user.inside) {
        user.temp_global[0] = user.global[0];
        user.temp_global[1] = user.global[1];
        if(key_states[SDL_SCANCODE_SPACE]) printf("Hello!\n");
        if(key_states[SDL_SCANCODE_W]) {
            user.global[1] -= (deltaTime/1000)*(user.speed - ((user.slowed)?(user.speed/1.2):0));//tilesize/4;
        }
        if(key_states[SDL_SCANCODE_A]) {
            user.global[0] -= (deltaTime/1000)*(user.speed - ((user.slowed)?(user.speed/1.2):0));//tilesize/4;
        }
        if(key_states[SDL_SCANCODE_S]) {
            user.global[1] += (deltaTime/1000)*(user.speed - ((user.slowed)?(user.speed/1.2):0));//tilesize/4;
        }
        if(key_states[SDL_SCANCODE_D]) {
            user.global[0] += (deltaTime/1000)*(user.speed - ((user.slowed)?(user.speed/1.2):0));//tilesize/4;
        }
        if(key_states[SDL_SCANCODE_P]) {
            explosion_click = !explosion_click;
            key_states[SDL_SCANCODE_P] = false;
        } 


        //update the view to only render visible tiles
        offset[0] = fmod(user.global[0], default_tilesize);
        offset[1] = fmod(user.global[1], default_tilesize);

        if(tilesize != default_tilesize){
            offset[0] = (fmod(user.global[0], static_cast<float>(default_tilesize)) / static_cast<float>(default_tilesize)) * static_cast<float>(tilesize);
            offset[1] = (fmod(user.global[1], static_cast<float>(default_tilesize)) / static_cast<float>(default_tilesize)) * static_cast<float>(tilesize);

            zoom_offsetx = (user.global[0] / static_cast<float>(default_tilesize)) - (user.global[0] / static_cast<float>(tilesize));
            zoom_offsety = (user.global[1] / static_cast<float>(default_tilesize)) - (user.global[1] / static_cast<float>(tilesize));
        }
        else{
            offset[0] = (fmod(user.global[0], static_cast<float>(tilesize)));
            offset[1] = (fmod(user.global[1], static_cast<float>(tilesize)));
            zoom_offsetx = 0;
            zoom_offsety = 0;
        }

        //Resolves issues with negative coordinates
        dx = (offset[0] < 0) ? tilesize + offset[0] : offset[0];
        dy = (offset[1] < 0) ? tilesize + offset[1] : offset[1];

        //Set the user's global tile
        user.tile[0] = static_cast<int>((((user.global[0])/default_tilesize)));
        user.tile[1] = static_cast<int>((((user.global[1])/default_tilesize)));

        //Get the terrain type of the user's tile
        n = WU.terrainGeneration(user.tile[0], user.tile[1]);
        //If the terrain is water, slow the user's speed
        if(n > 0.45)
            if(user.slowed) user.slowed = false;
        if(n < 0.45) user.slowed = true;

        
        //Set the corner tiles based on the user's global position
        view[0][0] = (user.tile[0] - ((WIDTH/tilesize)/2));// + zoom_offsetx; 
        view[0][1] = (user.tile[1] - ((HEIGHT/tilesize)/2));// + zoom_offsety;
        view[1][0] = view[0][0]+(WIDTH / tilesize);
        view[1][1] = view[0][1];
        view[2][0] = view[0][0];
        view[2][1] = view[0][1]+(HEIGHT / tilesize);
        view[3][0] = view[1][0];
        view[3][1] = view[2][1];

        //registry.clear<Tile>();
        visibleTiles.clear();
        clouds.clear();
        for(int x = view[0][0]; x < view[1][0] + 1; ++x) {
            for(int y = view[0][1]; y < view[3][1] + 2; ++y) {
                ID = hasher(to_string(x)+","+to_string(y));
                // auto it = find(TileIDs.begin(), TileIDs.end(), ID);
                // if(it != TileIDs.end()) {
                //     viewTiles[ID].x = (-dx + ((x-view[0][0])*tilesize));
                //     viewTiles[ID].y = (-dy + ((y-view[0][1])*tilesize));
                    
                //     visibleTiles.push_back(viewTiles[ID]);        
                // }
                //else {
                    //TileIDs.push_back(ID);
                    srand(ID);            
                    w = (-dx + ((x-view[0][0])*tilesize));
                    h = (-dy + ((y-view[0][1])*tilesize));

                    //const auto entity = registry.create();
                    n = WU.terrainGeneration(x, y);
                    rgb = 256 * n;
                    if(n < 0.45){
                        r = 0; g = 123; b = 238; //water
                    }
                    else if(n < 0.49){
                        r = rgb*1.8; g = rgb*1.7; b = rgb*1.2; 
                    } 
                    else if(n < 0.62){
                        r = 139*n*1.2; g = 214*n*1.8; b = 74*1.5; 
                        if(rand() % 1000 < 10) {
                            auto it = find(ObjIDs.begin(), ObjIDs.end(), ID);
                            if(it == ObjIDs.end()) {
                                ObjIDs.push_back(ID);
                                const auto plant = registry.create();
                                registry.emplace<Object>(plant, x, y, w, h, 1, ID);
                            }
                        }
                    }
                    else {
                        r = 139*(n*1.6); g = 139*(n*1.6); b = 139*(n*1.6);
                        if(rand() % 1000 < 1) {
                            auto objid = find(ObjIDs.begin(), ObjIDs.end(), ID);
                            if(objid == ObjIDs.end()) {
                                // auto objtimer = find(obj_spawn_timers.begin(), obj_spawn_timers.end(), static_cast<int>(ID));
                                // if(objtimer != obj_spawn_timers.end()) {
                                //     if(SDL_GetTicks() - obj_spawn_timers[ID] > 5000) {
                                //         obj_spawn_timers.erase(objtimer);
                                //     }
                                // }
                                // else {
                                //     ObjIDs.push_back(ID);
                                //     const auto rock = registry.create();
                                //     registry.emplace<Object>(rock, x, y, w, h, 0, ID);
                                // }
                                
                            }
                        }
                    }

                    //registry.emplace<Tile>(entity, w, h, x, y, n, r, g, b);
                    Tile tile{w, h, x, y, n, r, g, b, ID};
                    //viewTiles[ID] = tile;
                    visibleTiles.push_back(tile);
                    
                    srand(tile.ID+ctx->iteration);
                    w = tile.gx - (ctx->iteration/20);
                    h = tile.gy - (ctx->iteration/20);

                    // if(ctx->iteration%1234) w*=-1;
                    // if(ctx->iteration%123) h*=-1;
                    //WU.cloud_noise.SetFractalGain((rand()%10)*.01);
                    //tile.n = WU.cloudGeneration(w, h);
                    tile.n = WU.cloudGeneration(w, h);
                    if(tile.n < 0.69 && tile.n > 0.36) {
                        w+=tile.n*tilesize;
                        h+=tile.n*tilesize;
                        rgb=64+(224*tile.n);
                        Tile cloud{tile.x, tile.y, w, h, n, rgb,rgb,rgb, tile.ID};
                        clouds.push_back(cloud);
                    }

                                
                    
            // }
            }
        }
        // //Update Entities, EnTT!
        // auto tiles = registry.view<const Tile>();
        // auto sparkles = registry.view<const Effect>();
        auto buildings = registry.view<const Building>();
        auto objects = registry.view<const Object>();
        // //printf("%lu\n", buildings.size());
        // //Update visible tiles
        // //retain calculated tiles offscreen
        // if(buildings.size() == 0) {
        //     //noise.SetSeed(time(NULL));
        //     //WU.terrain_noise.SetSeed(time(NULL));
        //     //loadEntities();
        // }
        // for(auto [entity, tile]: tiles.each()) {
        //     tex_rect.x = tile.x; tex_rect.y = tile.y;
        //     tex_rect.w = tilesize; tex_rect.h = tilesize;
            
        //     SDL_SetRenderDrawColor(renderer,tile.r, tile.g, tile.b,255);
            
        //     SDL_RenderFillRect(renderer, &tex_rect);
        // }  
        
        for(auto tile : visibleTiles) {
            
            tex_rect.x = tile.x;// + abs((WIDTH/default_tilesize)/2) - ((WIDTH/tilesize)/2);
            tex_rect.y = tile.y;// + abs((HEIGHT/default_tilesize)/2) - ((HEIGHT/tilesize)/2);
            tex_rect.w = tilesize; tex_rect.h = tilesize;
            SDL_SetRenderDrawColor(renderer, tile.r, tile.g, tile.b,255);
            SDL_RenderFillRect(renderer, &tex_rect);

            //if(tilesize < 16) {
            //SDL_SetRenderDrawColor(renderer, 25, 25, 25, 160-tilesize);
            //}
            
        }  

        //Draw Buildings
        for(auto [entity, building]: buildings.each()) {
            //printf("%d %d\n", building.gx, view[0][0]*tilesize);
            if(user.inside) {
                user.temp_inside[0] = user.inside_pos[0];
                user.temp_inside[1] = user.inside_pos[1];

            }
            else if(building.gx + building.width >= view[0][0] && building.gx + building.width <= view[1][0] + building.width) {
                if(building.gy + building.height >= view[0][1] && building.gy + building.height <= view[2][1] + building.height) {
                    //Draw the main structure
                    w = (-dx + ((building.gx-view[0][0])*tilesize));// + abs((WIDTH/default_tilesize)/2) - ((WIDTH/tilesize)/2);
                    h = (-dy + ((building.gy-view[0][1])*tilesize));// + abs((HEIGHT/default_tilesize)/2) - ((HEIGHT/tilesize)/2);
                    tex_rect.x = w; tex_rect.y = h;
                    tex_rect.w = building.width*tilesize; tex_rect.h = building.height*tilesize;
                    SDL_SetRenderDrawColor(renderer, 115, 81, 60, 255);
                    SDL_RenderFillRect(renderer, &tex_rect);

                    //Draw the door
                    tex_rect.x = w + (building.doorx*tilesize); tex_rect.y = h + (building.doory*tilesize);
                    tex_rect.w = tilesize; tex_rect.h = tilesize;
                    SDL_SetRenderDrawColor(renderer, 180, 140, 97, 255);        
  
                    //cancel this render if the user walks into a building wall
                    if(user.tile[0] >= building.gx && user.tile[0] <= building.gx + building.width-1) {
                        if(user.tile[1] >= building.gy && user.tile[1] <= building.gy + building.height-1) {
                            //printf("WOWZA!\n");
                            user.global[0] = user.temp_global[0];
                            user.global[1] = user.temp_global[1];
                            return;
                        }
                    }

                    SDL_RenderFillRect(renderer, &tex_rect);

                    if(user.tile[0] >= building.gx + building.doorx && user.tile[0] <= building.gx + building.doorx+1) {
                        if(user.tile[1] >= building.gy + building.doory && user.tile[1] <= building.gy + building.doory+1) {
                            printf("DOOR!\n");
                            drawText(tex_rect, "Enter?", {tex_rect.x, tex_rect.y-tilesize/2}, tilesize, tilesize/2, ctx);
                            //Check if the user is clicking on the door
                            if(user.mouse[0] < (w + (building.doorx*tilesize))+tilesize && user.mouse[0] > w + (building.doorx*tilesize)) {
                                if(user.mouse[1] < (h + (building.doory*tilesize))+tilesize && user.mouse[1] > h + (building.doory*tilesize)) {
                                    SDL_SetRenderDrawColor(renderer, 255, 140, 97, 255);

                                    if(user.mousedown) {
                                        user.inside = true;
                                        user.inside_pos[0] = building.doorx;
                                        user.inside_pos[1] = building.doory-1;
                                        user.building = building;
                                        //registry.destroy(entity);
                                        continue;
                                    }
                                }
                            }
                        }
                    }

                    
                    //Draw sign
                    drawText(tex_rect, to_string(building.ID), {(w + (building.doorx*tilesize))-tilesize, h + (building.doory*tilesize)}, tilesize, tilesize/2, ctx);
                }
            }   
        }

        for(auto [entity, object]: objects.each()) {
            if(object.x >= view[0][0] && object.x <= view[1][0]) {
               if(object.y >= view[0][1] && object.y <= view[2][1]) {
                    w = (-dx + ((object.x-view[0][0])*tilesize));
                    h = (-dy + ((object.y-view[0][1])*tilesize));
                    if(user.mouse[0] < w + tilesize && user.mouse[0] > w) {
                        if(user.mouse[1] < h + tilesize && user.mouse[1] > h) {
                            tex_rect.x = w; tex_rect.y = h;
                            tex_rect.w = tilesize; tex_rect.h = tilesize/2;
                            SDL_SetRenderDrawColor(renderer, 222, 23, 97, 255);
                            SDL_RenderFillRect(renderer, &tex_rect);

                            if(user.mousedown) {
                                user.inventory[object.type]++;
                                registry.destroy(entity);
                                // auto it = find(obj_spawn_timers.begin(), obj_spawn_timers.end(), object.ID);
                                // if(it != obj_spawn_timers.end()) {
                                //     obj_spawn_timers[object.ID] = SDL_GetTicks();
                                //     ObjIDs.erase(ObjIDs.begin()+(it-ObjIDs.begin()));
                                //     registry.destroy(entity);
                                // }
                            }
                        }
                    }
                    drawText(tex_rect, obj_types[object.type], {w, h}, tilesize, tilesize/2, ctx);
                } 
            }            
        }

        for(auto cloud : clouds) {
            tex_rect.x = cloud.x;
            tex_rect.y = cloud.y;
            tex_rect.w = tilesize; tex_rect.h = tilesize;
            SDL_SetRenderDrawColor(renderer, 25, 25, 25, 160-tilesize);
            if(tilesize < 8) {
                rgb = 256*cloud.n;
                SDL_SetRenderDrawColor(renderer, rgb, rgb, rgb, 128+(128*cloud.n));
            }
            SDL_RenderFillRect(renderer, &tex_rect);
        }
        //User Rendering
        //w = (-dx + ((user.tile[0]-view[0][0])*tilesize));
        //h = (-dy + ((user.tile[1]-view[0][1])*tilesize));
        w = WIDTH/2;
        h = HEIGHT/2;
        tex_rect.x = w - tilesize/8; tex_rect.y = h - tilesize/8;
        tex_rect.w = tilesize/4; tex_rect.h = tilesize/4;
        SDL_SetRenderDrawColor(renderer, 255, 111, 97, 255);
        SDL_RenderFillRect(renderer, &tex_rect);
    }
    else {
        user.temp_inside[0] = user.inside_pos[0];
        user.temp_inside[1] = user.inside_pos[1];

        if(key_states[SDL_SCANCODE_SPACE]) printf("Hello!\n");
        if(key_states[SDL_SCANCODE_W]) {
            //printf("%f\n", user.global[1]);
            user.inside_pos[1]-=.1;
        }
        if(key_states[SDL_SCANCODE_A]) {
            user.inside_pos[0]-=.1;
        }
        if(key_states[SDL_SCANCODE_S]) {
            user.inside_pos[1]+=.1;
        }
        if(key_states[SDL_SCANCODE_D]) {
            user.inside_pos[0]+=.1;
        }

        if(user.inside_pos[0] <= 0 || user.inside_pos[0] >= user.building.width
            || user.inside_pos[1] <= 0 || user.inside_pos[1] >= user.building.height) {
            printf("OUT\n");
            user.inside_pos[0] = user.temp_inside[0];
            user.inside_pos[1] = user.temp_inside[1];
        }
        if(user.inside_pos[0] >= user.building.doorx && user.inside_pos[0] <= (1+user.building.doorx)
            && user.inside_pos[1] >= user.building.doory && user.inside_pos[1] <= (1+user.building.doory)) {
            printf("Leaving\n");
            drawText(tex_rect, "Exit?", 
                {
                    (user.building.doorx*tilesize) + (((WIDTH/tilesize)/2)-user.building.width/2)*tilesize, 
                    0
                },
                tilesize, tilesize/2, ctx);
            if(user.mousedown) {
                user.inside = false;
                return;
            }
        }
        //Empty the visible tile queue
        visibleTiles.clear();
        for(int x = 0; x < user.building.width; ++x) {
            for(int y = 0; y < user.building.height; ++y) {
                ID = hasher(to_string(x)+","+to_string(y));
                srand(ID);            
                
                //const auto entity = registry.create();
                n = WU.roomGeneration(x, y);
                rgb = 256 * n;

                if(n < 0.45){
                    r = 0; g = 123; b = 238; //water
                }
                else if(n < 0.49){
                    r = rgb*1.8; g = rgb*1.7; b = rgb*1.2; 
                } 
                else if(n < 0.52){
                    r = 0; g = rgb; b = rgb*.4;
                }
                else {
                    r = n*255; g = n*255; b = n*255;
                }
                            
                //registry.emplace<Tile>(entity, w, h, x, y, n, r, g, b);
                Tile tile{x*tilesize, y*tilesize, x, y, n, r, g, b, ID};
                //viewTiles[ID] = tile;
                visibleTiles.push_back(tile);
            }
        }
        
        for(auto tile : visibleTiles) {
            srand(user.building.ID);

            tex_rect.x = 64 * (rand() % 7);
            tex_rect.y = 96 * (rand() % 4);

            if(tile.x == 0) {
                if(tile.y != 0) {
                    tex_rect.y += 32;
                }
            }
            else if(tile.y == 0) {
                tex_rect.x += 32;
            }
            else {
                tex_rect.x += 32;
                tex_rect.y += 32;
            }

            tex_rect.w = 32; tex_rect.h = 32;

            screen_rect.x = tile.x + (((WIDTH/tilesize)/2)-user.building.width/2)*tilesize; screen_rect.y = tile.y + (((HEIGHT/tilesize)/2)-user.building.height/2)*tilesize;
            screen_rect.w = tilesize; screen_rect.h = tilesize;
            SDL_RenderCopy(renderer, TU.Textures["interior"][1].tex, &tex_rect, &screen_rect); //static_cast<int>(sparkle.time/6)%4

            // SDL_SetRenderDrawColor(renderer,tile.r, tile.g, tile.b,255);
            // SDL_RenderFillRect(renderer, &tex_rect);
        }  
        
        for(auto object: user.building.objects) {
            w = object.x*tilesize + (((WIDTH/tilesize)/2)-user.building.width/2)*tilesize;
            h = object.y*tilesize + (((HEIGHT/tilesize)/2)-user.building.height/2)*tilesize;
            if(user.mouse[0] < w + tilesize && user.mouse[0] > w) {
                if(user.mouse[1] < h + tilesize && user.mouse[1] > h) {
                    tex_rect.x = w; tex_rect.y = h;
                    tex_rect.w = tilesize; tex_rect.h = tilesize/2;
                    SDL_SetRenderDrawColor(renderer, 222, 23, 97, 255);
                    SDL_RenderFillRect(renderer, &tex_rect);

                    if(user.mousedown) {
                        user.inventory[object.type]++;
                        //registry.destroy(entity);
                        // auto it = find(obj_spawn_timers.begin(), obj_spawn_timers.end(), object.ID);
                        // if(it != obj_spawn_timers.end()) {
                        //     obj_spawn_timers[object.ID] = SDL_GetTicks();
                        //     ObjIDs.erase(ObjIDs.begin()+(it-ObjIDs.begin()));
                        //     registry.destroy(entity);
                        // }
                    }
                }
            }
            drawText(tex_rect, obj_types[object.type], {w, h}, tilesize, tilesize/2, ctx);
        }

        tex_rect.x = user.building.doorx*tilesize + (((WIDTH/tilesize)/2)-user.building.width/2)*tilesize;
        tex_rect.y = user.building.doory*tilesize + (((HEIGHT/tilesize)/2)-user.building.height/2)*tilesize;
        tex_rect.w = tilesize; tex_rect.h = tilesize;
        SDL_SetRenderDrawColor(renderer,255, 255, 255,255);
        SDL_RenderFillRect(renderer, &tex_rect);

        //User Rendering
        //w = (-dx + ((user.tile[0]-view[0][0])*tilesize));
        //h = (-dy + ((user.tile[1]-view[0][1])*tilesize));
        w = WIDTH/2;
        h = HEIGHT/2;
        tex_rect.x = (user.inside_pos[0]*tilesize)+(((WIDTH/tilesize)/2)-user.building.width/2)*tilesize; 
        tex_rect.y = (user.inside_pos[1]*tilesize)+(((HEIGHT/tilesize)/2)-user.building.height/2)*tilesize;
        tex_rect.w = tilesize/4; tex_rect.h = tilesize/4;
        SDL_SetRenderDrawColor(renderer, 255, 111, 97, 255);
        SDL_RenderFillRect(renderer, &tex_rect);
    }