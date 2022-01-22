#include <SDL2/SDL_image.h>
#include <emscripten.h>
#include <unordered_map>
#include <vector>
#include <string>

struct Texture
{
    std::string name;
    int w;
    int h;
    SDL_Texture *tex = NULL;
};

struct SubTexture
{
    SDL_Rect screen;
    SDL_Rect texture;
    int resource_index;
    bool special;
    int type;
};

class TextureUtils {
    public:
        SDL_Renderer *renderer;
        std::unordered_map<std::string, std::vector<Texture>> Textures;

        bool texturesLoaded;
        bool loadTextures();
};