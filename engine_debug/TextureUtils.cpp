#include "TextureUtils.h"


//TODO
//set file paths and types in a separate file
//read from that file to load textures
std::vector<std::string> star_effects {
    "Assets/0000.png",
    "Assets/0001.png",
    "Assets/0002.png",
    "Assets/0003.png",
    "Assets/0004.png",
};
std::vector<std::string> explosions {
    "Assets/Explosion.png",
};
std::vector<std::string> food {
    "Assets/dish1.png",
    "Assets/dishtop.png",
    "Assets/dishpile.png",
    "Assets/pizza.png",
    "Assets/100_taco_dish.png"
};
std::vector<std::string> interior {
    "Assets/interiors.png",
    "Assets/floors.png"
};
std::vector<std::string> misc {
    "Assets/tf.png"
};
std::unordered_map<std::string, std::vector<std::string>> texture_types{
    {"sparkles", star_effects},
    {"food", food},
    {"boom", explosions},
    {"interior", interior},
    {"misc", misc}
};

//,{"items", image_files} {"animals", animal_files}, 
bool TextureUtils::loadTextures(){
    printf("Loading Textures...\n");
    if(texturesLoaded) return true;
    else{
        SDL_Texture *img = NULL;
        for(auto tex_type : texture_types){
            for(int i = 0; i < tex_type.second.size(); ++i){
                
                img = IMG_LoadTexture(renderer, tex_type.second[i].c_str());
                printf((img == NULL) ? "Image failed to load\n" : "Good\n");
                Textures[tex_type.first].push_back(Texture{tex_type.second[i].c_str(), 0, 0, img});
                SDL_QueryTexture(Textures[tex_type.first][i].tex, NULL, NULL, 
                    &Textures[tex_type.first][i].w, &Textures[tex_type.first][i].h
                );
            }
        }

        if(Textures.size() == texture_types.size()){
            texturesLoaded = true;
            return true;
        }
        else return false;
    }
    printf(" Textures Loaded.\n");
}

