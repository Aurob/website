
#pragma once
#include "../libs/raylib/raylib.h"
#include "../libs/raylib/raymath.h"
#include "../libs/raylib/rlgl.h"
#include "../libs/flecs/flecs.h"
#include "../libs/cJSON/cJSON.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>  

#define GLSL_VERSION 100

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY } GameScreen;
extern Camera camera;
void LoadTextures(void);
void LoadECS(void);
void CloseECS(void);
void LoadEntities(void);
void CheckCollisions(void);
void CheckInteractions(ecs_iter_t* it);
void UpdateSceneCamera(ecs_iter_t* it);
void DRAW_ECS(Camera* camera);
void ECS_PROGRESS(Camera *camera);
void UnloadECS();
