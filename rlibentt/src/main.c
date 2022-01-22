/*******************************************************************************************
*
*   raylib Game Template
*
*   <Game title>
*   <Game description>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "../libs/raylib/raylib.h"
#include "rlgl.h"
#include "ecs_main.h"
// #include "screens.h"


#define RLIGHTS_IMPLEMENTATION
#include "../libs/raylib/rlights.h"
#include <emscripten/emscripten.h>
#define GLSL_VERSION            100

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;


GameScreen screen = 0;
Camera camera = { 0 };

Texture2D bill;
Vector3 billPosition;

Shader shader;
Model model;
Light light;

Mesh cube;
Model skybox;

Model roach;
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void); 

int main(void)
{
    // Initialization
    //---------------------------------------------------------
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Raylib Prototype v2");
    InitAudioDevice();      // Initialize audio device

    printf("Loading default camera\n");
    
    LoadTextures();
    camera.position = (Vector3){ 4.0f, 12.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 12.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.walkspeed = 25; 
    LoadECS();

    // Load plane model from a generated mesh
    model = LoadModelFromMesh(GenMeshPlane(100.0f, 100.0f, 3, 3));
    roach = LoadModel("/var/www/html/test/resources/models/bookpile.glb");
    shader = LoadShader(TextFormat("/var/www/html/test/resources/shaders/base_lighting.vs", GLSL_VERSION),
                        TextFormat("/var/www/html/test/resources/shaders/lighting.fs", GLSL_VERSION));

    // Get some required shader loactions
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading, 
    // no need to get the location again if using that uniform name
    // shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    
    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);

    // Assign out lighting shader to model
    roach.materials[0].shader = shader;
    light = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, -2 }, Vector3Zero(), BLUE, shader);

    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode
    
    

#if defined(PLATFORM_WEB)
    // emscripten_run_script("alert('hi')");
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadECS();
    UnloadModel(model);     // Unload the model
    UnloadShader(shader);   // Unload shader

    // Unload all global loaded data (i.e. fonts) here!
    CloseECS();
    CloseAudioDevice();     // Close audio context
    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

void UpdateDrawFrame() {
    //Crouching
    if(IsKeyDown(KEY_LEFT_SHIFT)) {
        camera.position.y = 7;
    }
    else camera.position.y = 12;

    ECS_PROGRESS(&camera);    
    UpdatePosition(&camera);
    
    CheckCollisions();
    ExUpdatePosition(&camera);

    UpdateLightValues(shader, light);
    // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
    float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
    BeginDrawing();
        ClearBackground(BLACK);
        DRAW_ECS(&camera);

        BeginMode3D(camera);
        // DrawModel(roach, Vector3Zero(), 1.0f, WHITE);
        
        // DrawModel(roach, (Vector3){0, 10, 0}, .1f, WHITE);
        EndMode3D();
    EndDrawing();

    
    // printf("%f %f\n", camera.position.x, camera.position.z);
    
}
