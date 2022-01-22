#include <emscripten.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <map>
#include "Game.h"

using namespace std;


struct context {
    SDL_Renderer *renderer;
    SDL_Event event;
    int iteration;
};
SDL_Rect tex_rect;
SDL_Rect screen_rect;
Game game;
// EM_JS(void, send_event, (int x, int y, int viewx, int viewy, int vieww, int viewh), {
//   eval('socket.emit("client_update", {"user":socket.id, "position":['+x+','+y+']});');
// });

void drawText(SDL_Rect & text_rect, string text, pair<float, float> position, int width, int height, context * ctx) {
    
    SDL_Color fg = {0,0,0};
    SDL_Color bg = {100,100,100};
    SDL_Color color = { 255, 255, 255 };
    TTF_Font *font = TTF_OpenFont("Assets/LemonMilkRegular.otf", 16);
    if(!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        // handle error
    }
    SDL_Surface * surface = NULL;
    SDL_Texture * texture = NULL;
    
    text_rect.x = position.first;
    text_rect.y = position.second;
    text_rect.w = width;    
    text_rect.h = height;
    //string data{};
    surface = TTF_RenderText_Solid(font, text.c_str(), color);
    texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
    SDL_RenderCopy(ctx->renderer, texture, NULL, &text_rect);

    TTF_CloseFont(font);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void mainloop(void *arg)
{   
    
    //printf("%f\n", deltaTime/1000);
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    //Handle events
    
    while (SDL_PollEvent(&ctx->event)) {
        game.EventHandler(0, &ctx->event);
    }
    game.UpdateView();
    game.UpdateEffects();
    game.UpdateUI();
    
    //Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
    SDL_RenderClear(renderer);

    //Draw tiles in the view
    for(auto & tile : game.visibleTiles) {
        tex_rect.x = tile.x;
        tex_rect.y = tile.y;
        tex_rect.w = game.tilesize; tex_rect.h = game.tilesize;
        SDL_SetRenderDrawColor(renderer, tile.r, tile.g, tile.b,255);
        SDL_RenderFillRect(renderer, &tex_rect);
    } 
    
    //User sprite
    tex_rect.x = WIDTH/2 - game.tilesize/8; tex_rect.y = HEIGHT/2 - game.tilesize/8;
    tex_rect.w = game.tilesize/4; tex_rect.h = game.tilesize/4;
    SDL_SetRenderDrawColor(renderer, 255, 111, 97, 255);
    SDL_RenderFillRect(renderer, &tex_rect);

    game.registry.sort<Object>([](const auto &lhs, const auto &rhs) { 
        return lhs.y < rhs.y;
    });

    auto objects = game.registry.view<const Object>();
    for(auto [entity, object]: objects.each()) {
        if(!object.rendered) continue;
        if(object.type == 5) {
            tex_rect.x = object.sx; 
            tex_rect.y = object.sy;
            tex_rect.w = game.tilesize; tex_rect.h = game.tilesize;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &tex_rect);
        }
        if(object.type == 4) {
            //TODO
            //Add shadows that reflect the time of day
            //i.e. adjust the width and direction
            SDL_SetRenderDrawColor(renderer, 105, 105, 105, 50);
            if(game.dayTime <= 30000) {
                //trunk shadow morning
                tex_rect.x = (object.sx - object.h*game.tilesize) + ((static_cast<float>(game.dayTime)/30000)*static_cast<float>(object.h))*game.tilesize;
                tex_rect.y = object.sy;
                tex_rect.w = object.sx - tex_rect.x;
                tex_rect.h = game.tilesize;
                SDL_RenderFillRect(renderer, &tex_rect);

                //canopy shadow morning
                // tex_rect.x = tex_rect.x - (game.tilesize*5)/2; 
                // tex_rect.y = tex_rect.y - (game.tilesize*4)/2;
                // tex_rect.w = game.tilesize*5; tex_rect.h = game.tilesize*4;
                // SDL_RenderFillRect(renderer, &tex_rect);
            }
            else if(game.dayTime <= 60000) {
                //trunk shadow afternoon
                tex_rect.x = object.sx; 
                tex_rect.y = object.sy;
                tex_rect.w = ((static_cast<float>(game.dayTime-30000)/30000)*static_cast<float>(object.h))*game.tilesize;
                tex_rect.h = game.tilesize;
                SDL_RenderFillRect(renderer, &tex_rect);

                //canopy shadow afternoon
                // tex_rect.x = tex_rect.x; 
                // tex_rect.y = tex_rect.y - (game.tilesize*4)/2;
                // tex_rect.w = game.tilesize*5; tex_rect.h = game.tilesize*4;
                // SDL_RenderFillRect(renderer, &tex_rect);
            }           

            //Draw tree canopy
            tex_rect.x = object.sx - game.tilesize*2; 
            tex_rect.y = (object.sy - (game.tilesize*object.h)) - game.tilesize*4;
            tex_rect.w = game.tilesize*5; tex_rect.h = game.tilesize*4;
            SDL_SetRenderDrawColor(renderer, 45, 113, 37, 255);
            SDL_RenderFillRect(renderer, &tex_rect);
            //Draw tree trunk
            tex_rect.x = object.sx; tex_rect.y = object.sy - game.tilesize*object.h;
            tex_rect.w = game.tilesize; tex_rect.h = (game.tilesize*object.h)+game.tilesize;
            SDL_SetRenderDrawColor(renderer, 105, 78, 61, 255);
            SDL_RenderFillRect(renderer, &tex_rect);
        }
    }

    //Draw day time filter
    //Simple rectangle with transparency
    // tex_rect.x = 0; tex_rect.y = 0;
    // tex_rect.w = WIDTH; tex_rect.h = HEIGHT;
    // SDL_SetRenderDrawColor(renderer, range(60000-(game.dayTime), 0, 60000, 0, 255), 24, 22, 150);
    // SDL_RenderFillRect(renderer, &tex_rect);

    //Draw Effects
    //Starting with sparkles
    auto sparkles = game.registry.view<const Effect>();
    for(auto [entity, sparkle]: sparkles.each()) {
        if(SDL_GetTicks() - sparkle.time > 500) {
            game.registry.destroy(entity);
            continue;
        }
        tex_rect.x = 0; tex_rect.y = 0;
        tex_rect.w = 57; tex_rect.h = 52;
        screen_rect.x = sparkle.x; screen_rect.y = sparkle.y;
        screen_rect.w = game.tilesize; screen_rect.h = game.tilesize;
        SDL_RenderCopy(renderer, game.TU.Textures["sparkles"][static_cast<int>((SDL_GetTicks() - sparkle.time)/6)%4].tex, &tex_rect, &screen_rect);
    }
    
    //GUI Rendering
    //Draw GUI show/hide button
    tex_rect.x = 0; tex_rect.y = 0;
    tex_rect.w = 140; tex_rect.h = 20;
    SDL_SetRenderDrawColor(renderer, 255, 111, 97, 255);
    SDL_RenderFillRect(renderer, &tex_rect);

    //Draw the menu items
    int info_h = 0;
    for(string text : game.info_list) {
        drawText(tex_rect, text, {0, info_h}, 140, 20, ctx);
        info_h += 20;
    }

    auto popups = game.registry.view<const UIBlock>();
    for(auto [entity, popup]: popups.each()) {
        if(popup.ID == 1234) printf("%d\n", popup.shown);
        if(!popup.shown) continue;

        tex_rect.x = popup.x; tex_rect.y = popup.y;
        tex_rect.w = popup.w; tex_rect.h = popup.h;
        SDL_SetRenderDrawColor(renderer, 255, 111, 97, 225);
        SDL_RenderFillRect(renderer, &tex_rect);

        if(popup.type == 2) {
            //Move button
            tex_rect.x = popup.x; tex_rect.y = popup.y;
            tex_rect.w = popup.w; tex_rect.h = popup.h/16;
            SDL_SetRenderDrawColor(renderer, 20, 122, 255, 255);
            SDL_RenderFillRect(renderer, &tex_rect);
            //Close button
            tex_rect.x = popup.x+(popup.w-(popup.w/16)); tex_rect.y = popup.y+(popup.h/16);
            tex_rect.w = popup.w/16; tex_rect.h = popup.h/16;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &tex_rect);
            //Pin button
            // tex_rect.x = popup.x+(popup.w-(popup.w/16)); tex_rect.y = popup.y+(popup.h/8);
            // tex_rect.w = popup.w/16; tex_rect.h = popup.h/16;
            // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            // SDL_RenderFillRect(renderer, &tex_rect);
        }
        else if(popup.type == 1) {

        }
        // tex_rect.x = 0, tex_rect.y = 0;
        // tex_rect.w = 1073; tex_rect.h = 1303;
        // screen_rect.x = popup.x+popup.w/8; screen_rect.y = popup.y+popup.h/4;
        // screen_rect.w = popup.w*2 - popup.w/4; screen_rect.h = popup.h*2 - popup.h/4;
        // SDL_RenderCopy(renderer, game.TU.Textures["misc"][0].tex, &tex_rect, &screen_rect); //static_cast<int>(sparkle.time/6)%4

        int text_h = popup.h/16;
        for(auto text : popup.content) {
            drawText(tex_rect, text.first, {popup.x + popup.w/32, popup.y+text_h}, (popup.w - popup.w/8) / text.second, popup.h/16, ctx);
            text_h+=(popup.h/16);
        }
    }

   if(game.user.can_interact){
        tex_rect.x = game.user.mouse[0]; tex_rect.y = game.user.mouse[1];
        tex_rect.w = game.tilesize/5; tex_rect.h = game.tilesize/5;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &tex_rect);
    }
    
    // send_event(user.global[0], user.global[1]);
    SDL_RenderPresent(renderer);
    ctx->iteration++;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    //Initialize the Game object
    TTF_Init();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    //Allows opacity
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    context ctx;
    ctx.renderer = renderer;
    game.TU.renderer = ctx.renderer;
    game.TU.loadTextures();
    
    // auto b = registry.create();
    // auto &building = registry.emplace<Building>(b);
    // building.gx=33-5;building.gy=45-9;
    // building.doorx=5;building.doory=9;
    // building.width=10;
    // building.height=10;
    // user.inside = true;
    // user.building = building;
    
    // user.global[0] = building.gx*tilesize;
    // user.global[1] = building.gy*tilesize;
    //Remove timer in case the call back was not called
    // lastTime = SDL_GetTicks();
    emscripten_set_main_loop_arg(mainloop, &ctx, -1, 1);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}