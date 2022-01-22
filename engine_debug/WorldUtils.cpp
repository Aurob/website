#include "WorldUtils.h"

WorldUtils::WorldUtils(){
    //Set noise specific presets
    terrain_noise.SetSeed(time(NULL));
    cloud_noise.SetSeed(time(NULL));
    cloud_noise.SetFrequency(.001);
}

float WorldUtils::terrainGeneration(int i, int j){
    //
    // n = terrain_noise.GetPerlinFractal(i, j, 0, 1, 1, 0);
    //n = (terrain_noise.GetPerlinFractal((i), (j)) - -1) / (1 - -1);
    //n = (terrain_noise.GetPerlinFractal((i)+pow(n,2), (j)+pow(n,2)) - -1) / (1 - -1);

    return (terrain_noise.GetPerlinFractal((i), (j)) - -1) / (1 - -1);
}

float WorldUtils::cloudGeneration(int i, int j){
    //
    //
    return (cloud_noise.GetSimplexFractal((i), (j)) - -1) / (1 - -1);
}

float WorldUtils::roomGeneration(int i, int j){
    //
    //n = (room_noise.GetPerlinFractal((i), (j)) - -1) / (1 - -1);
    //n = (terrain_noise.GetPerlinFractal((i)+pow(n,2), (j)+pow(n,2)) - -1) / (1 - -1); 
    return (room_noise.GetPerlinFractal((i), (j)) - -1) / (1 - -1);
}
