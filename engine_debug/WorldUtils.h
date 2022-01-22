#include <unordered_map>
#include <vector>
#include <string>
#include <time.h>
#include "FastNoise.h"

using namespace std;

class WorldUtils {
    public:
        float n;
        FastNoise terrain_noise;
        FastNoise room_noise;
        FastNoise cloud_noise;
        WorldUtils();
        
        float terrainGeneration(int, int);
        float roomGeneration(int, int);
        float cloudGeneration(int, int);
};
