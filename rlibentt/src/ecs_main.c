#include "ecs_main.h"

typedef struct Scene {
    GameScreen id;
    char* name;
    double width, height, length;
    bool cursor;
    bool fps;
    bool grid;
} Scene;

typedef struct Position {
    double x, y, z;
} Position;

typedef struct Shape {
    float w, h, l;
    bool _3D;
    bool _billboard;
    Color color;
} Shape;

typedef struct Text {
    char *text;
    int size;
    Color color;
} Text;

typedef enum {RCLICK = 0, LCLICK, HOVER} InteractionType;
typedef struct Interaction {
    int type;
} Interaction;

typedef struct Teleport {
    Vector3 position;
} Teleport;

typedef struct Destroy {
    int id;
} Destroy;

typedef struct SceneCamera {
    Camera camera;
} SceneCamera;

typedef struct Color_ECS {
    int r, g, b;
} Color_ECS;

typedef struct Texture_ECS {
    const char* file_name;
    int index;
    Texture2D tex;
} Texture_ECS;

typedef struct Model_ECS { //gltk!
    const char* file_name;
    Model *model;
    float scale;
    int id;
    float rotatef, scalef, translatef;
} Model_ECS;

typedef struct Rotate {
    bool loop;
    float angle;
} Rotate;

ECS_COMPONENT_DECLARE(Scene);
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Shape);
ECS_COMPONENT_DECLARE(Text);
ECS_COMPONENT_DECLARE(Interaction);
ECS_COMPONENT_DECLARE(SceneCamera);
ECS_COMPONENT_DECLARE(Teleport);
ECS_COMPONENT_DECLARE(Destroy);
ECS_COMPONENT_DECLARE(Color_ECS);
ECS_COMPONENT_DECLARE(Texture_ECS);
ECS_COMPONENT_DECLARE(Model_ECS);
ECS_COMPONENT_DECLARE(Rotate);
ecs_world_t *world = NULL;
int gamescreen = 0;

const int MAX_COLLIDE_HEIGHT = 150;
/*
TESTING
*/
Texture2D test_img;
// Texture2D textures[1000];
// int tex_count = 0;
void ECS_PROGRESS(Camera *_camera) {
    // *_camera = camera;
    ecs_progress(world, 0);
    *_camera = camera;
}
void LoadTextures(void) {
    // test_img = LoadTexture("resources/cat.png");
}
void LoadECS(void) {
    world = ecs_init_w_args(0, NULL);

    ECS_COMPONENT_DEFINE(world, Scene);
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Shape);
    ECS_COMPONENT_DEFINE(world, Text);
    ECS_COMPONENT_DEFINE(world, Interaction);
    ECS_COMPONENT_DEFINE(world, SceneCamera);
    ECS_COMPONENT_DEFINE(world, Teleport);
    ECS_COMPONENT_DEFINE(world, Destroy);
    ECS_COMPONENT_DEFINE(world, Color_ECS);
    ECS_COMPONENT_DEFINE(world, Texture_ECS);
    ECS_COMPONENT_DEFINE(world, Model_ECS);
    ECS_COMPONENT_DEFINE(world, Rotate);
    //Load defined entities in test.json
    LoadEntities();
    ECS_SYSTEM(world, CheckInteractions, EcsPreUpdate, Position, Shape, Interaction, Scene);
    // ECS_SYSTEM(world, UpdateSceneCamera, EcsOnUpdate, Scene, SceneCamera);
    printf("ECS starting\n");
}

void CloseECS(void) {
    ecs_fini(world);
}
//Load entities from a JSON file
void LoadEntities(void) {

    //Load the entity definition JSON file
    FILE *fp;
	char buffer[81920];
    fp = fopen("/var/www/html/test/resources/scenes/build.json.min","r");
	fread(buffer, 81920, 1, fp);
	fclose(fp);

    //Use cJSON to load the string buffer into a cJSON object
    cJSON *json = cJSON_Parse(buffer);

    //Get the number of textures and create an array to store each texture
    int tex_count = cJSON_GetObjectItem(json, "tex_count")->valueint;
    Texture2D textures[tex_count];
    //Textures might not be in order, so store whether or not the texture has been loaded
    bool textures_init[tex_count];
    for(int i = 0; i < tex_count; ++i) {
        textures_init[i] = 0;
    }

    //Get the number of models and create an array to store each model
    int model_count = cJSON_GetObjectItem(json, "model_count")->valueint;
    Model models[model_count];
    //Textures might not be in order, so store whether or not the texture has been loaded
    bool models_init[model_count];
    for(int i = 0; i < model_count; ++i) {
        models_init[i] = 0;
    }

    //Scenes are used to define specific instances in which entities are located
    cJSON *scenes = cJSON_GetObjectItem(json, "Scenes");
    int scenes_count = cJSON_GetArraySize(scenes);
    for (int i = 0; i < scenes_count; i++) {
        cJSON *scene = cJSON_GetArrayItem(scenes, i);

        //Get the name and id values from the Scene object
        //TODO. should verify these items exist
        //      break if not        
        char *name = cJSON_GetObjectItem(scene, "name")->valuestring;
        int id = cJSON_GetObjectItem(scene, "id")->valueint;
        double width = cJSON_GetObjectItem(scene, "width")->valuedouble;
        double height = cJSON_GetObjectItem(scene, "height")->valuedouble;
        double length = cJSON_GetObjectItem(scene, "length")->valuedouble;
        bool cursor = cJSON_GetObjectItem(scene, "cursor")->valueint;
        bool fps = cJSON_GetObjectItem(scene, "fps")->valueint;
        bool grid = cJSON_GetObjectItem(scene, "grid")->valueint;
        printf("Scene name: %s, id: %d\n", name, id);

        //Check if the scene has an entity defintion array
        cJSON *entities = cJSON_GetObjectItem(scene, "entities");
        if (cJSON_IsArray(entities))
        {
            //Iterate each entity contained in the Scene
            int entity_count = cJSON_GetArraySize(entities);
            
            for (int j = 0; j < entity_count; j++) {
                cJSON *entity = cJSON_GetArrayItem(entities, j);
                
                //Check if the entity has a component defintion array
                cJSON *components = cJSON_GetObjectItem(entity, "components");
                if (cJSON_IsArray(components))
                {
                    //Create the new entity
                    ecs_entity_t e = ecs_new_id(world);
                    ecs_set_name(world, e, name);
                    ecs_set(world, e, Scene, {.id = id, .width = width, .height = height, .length = length, 
                                              .cursor = cursor, .fps = fps, .grid = grid});
                    //Iterate each defined component for the entity
                    int component_count = cJSON_GetArraySize(components);
                    for (int k = 0; k < component_count; k++) {
                        cJSON *component = cJSON_GetArrayItem(components, k);

                        //Add each defined component with values to the ECS world
                        switch(cJSON_GetObjectItem(component, "type")->valueint) {

                            case 0: { //Position
                                
                                cJSON *x_pos = cJSON_GetObjectItem(component, "x");
                                double x=0;
                                if(cJSON_IsNumber(x_pos)) {
                                    x = x_pos->valuedouble;
                                }
                                else if(cJSON_IsString(x_pos)) {
                                    char *alt_x = x_pos->valuestring;
                                    if(strcmp(alt_x, "w")==0) 
                                        x = GetScreenWidth();
                                    else if(strcmp(alt_x, "w/2")==0)
                                        x = GetScreenWidth()/2;
                                }

                                cJSON *y_pos = cJSON_GetObjectItem(component, "y");
                                double y=0;
                                if(cJSON_IsNumber(y_pos)) {
                                    y = y_pos->valuedouble;
                                }
                                else if(cJSON_IsString(y_pos)) {
                                    char *alt_y = y_pos->valuestring;
                                    if(strcmp(alt_y, "h")==0)
                                        y = GetScreenHeight();
                                    if(strcmp(alt_y, "h/2")==0)
                                        y = GetScreenHeight()/2;
                                }
                                
                                cJSON *z_pos = cJSON_GetObjectItem(component, "z");
                                double z=0;
                                if(cJSON_IsNumber(z_pos)) {
                                    z = z_pos->valuedouble;
                                }
                                ecs_set(world, e, Position, {.x = x, .y = y, .z = z});
                                break;
                            }

                            case 1: { //Shape
                                //TODO
                                //dynamic dimensions
                                double w = cJSON_GetObjectItem(component, "w")->valuedouble;
                                double h = cJSON_GetObjectItem(component, "h")->valuedouble;
                                cJSON *length = cJSON_GetObjectItem(component, "l");
                                if(cJSON_IsNumber(length)) {
                                    double l = cJSON_GetObjectItem(component, "l")->valuedouble;
                                    if(l > 0)
                                        ecs_set(world, e, Shape, {.w = w, .h = h, .l = l, ._3D = true});
                                    else ecs_set(world, e, Shape, {.w = w, .h = h, ._3D = false});   
                                }                            
                                else ecs_set(world, e, Shape, {.w = w, .h = h, ._3D = false});   
                                break;
                            }

                            case 2: { //Text
                                char *text = cJSON_GetObjectItem(component, "text")->valuestring;
                                int size = cJSON_GetObjectItem(component, "size")->valueint;
                                // printf("%s\n", text);
                                ecs_set(world, e, Text, {.text = text, .size = size});
                                break;
                            }

                            case 3: { //Interaction
                                int itype = cJSON_GetObjectItem(component, "interaction")->valueint;
                                ecs_set(world, e, Interaction, {.type = itype});
                                break;
                            }

                            case 4: { //Teleport (On Interaction)
                                double x = cJSON_GetObjectItem(component, "x")->valuedouble;
                                double y = cJSON_GetObjectItem(component, "y")->valuedouble;
                                double z = cJSON_GetObjectItem(component, "z")->valuedouble;
                                printf("teleportation location: %f %f %f\n", x, y, z);
                                //might not want to set 'from' here
                                ecs_set(world, e, Teleport, {.position = (Vector3){x, y, z}});

                                break;
                            }

                            case 5: { //SceneCamera
                                Camera camera = { 0 };
                                camera.position = (Vector3){ 0.0f, 7.0f, 0.0f };
                                camera.target = (Vector3){ 0.0f, 7.0f, 0.0f };
                                camera.up = (Vector3){ 0.0f, 10.0f, 0.0f };
                                camera.fovy = 60.0f;
                                camera.projection = CAMERA_PERSPECTIVE;
                                camera.walkspeed = 4;    
                                camera.oobx = false;
                                camera.oobz = false;
                                SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode
                                ecs_set(world, e, SceneCamera, {.camera = camera});
                                break;
                            }

                            case 6: { //Destroy (On Interaction)
                                ecs_set(world, e, Destroy, {.id=0});
                                break;
                            }

                            case 7 : { //Color (Raylib Color mapping)
                                int r = cJSON_GetObjectItem(component, "r")->valueint;
                                int g = cJSON_GetObjectItem(component, "g")->valueint;
                                int b = cJSON_GetObjectItem(component, "b")->valueint;
                                ecs_set(world, e, Color_ECS, {.r=r, .g=g, .b=b});
                                break;
                            }

                            case 8 : { //Texture
                                //TODO
                                //DO NOT STORE ANY TEXTURE MORE THAN ONCE
                                int tex_id = cJSON_GetObjectItem(component, "id")->valueint;
                                char *file_name = cJSON_GetObjectItem(component, "file_name")->valuestring; 
                                Texture2D temp_tex;
                                if(textures_init[tex_id]) {
                                    temp_tex = textures[tex_id];
                                }
                                else {
                                    temp_tex = LoadTexture(file_name);
                                    textures[tex_id] = temp_tex;
                                    textures_init[tex_id] = 1;
                                }
                                // temp_tex = LoadTexture(file_name);
                                
                                tex_count++;
                                ecs_set(world, e, Texture_ECS, {.file_name = file_name, .index = tex_id, .tex = temp_tex});
                                break;
                            }

                            case 9 : { //Model
                                char *file_name = cJSON_GetObjectItem(component, "file_name")->valuestring; 
                                int model_id = cJSON_GetObjectItem(component, "id")->valueint;
                                float scale = cJSON_GetObjectItem(component, "scale")->valuedouble;
                                // Model temp_model;
                                if(!models_init[model_id]) {
                                    models[model_id] = LoadModel(file_name);
                                    models_init[model_id] = 1;
                                }
                                // printf("The id and file is %d %s\n", model_id, file_name);
                                
                                model_count++;
                                ecs_set(world, e, Model_ECS, {.file_name = file_name, 
                                .scale = scale, .rotatef = 0, .scalef = 0, .translatef = 0,
                                .id = model_id, .model = &models[model_id]});
                                
                                // UnloadModel(temp_model);

                                break;
                            }

                            case 10 : { //Rotate (Transform?)
                                int loop = cJSON_GetObjectItem(component, "loop")->valueint;
                                float angle = cJSON_GetObjectItem(component, "angle")->valuedouble;
                                printf("is loop %d\n", loop);
                                ecs_set(world, e, Rotate, {.loop = loop, .angle = angle});
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
            }
        }
        else {
            printf("no\n");
        }

    }
}

void CheckCollisions() {
    ecs_query_t *q = ecs_query_new(world, "Position, Shape, Scene");
    ecs_iter_t it = ecs_query_iter(q);
    bool collision = false;
    while (ecs_query_next(&it)) {
        Position *pos = ecs_term(&it, Position, 1);
        Shape *shape = ecs_term(&it, Shape, 2);
        Scene *scene = ecs_term(&it, Scene, 3);
        
        for (int i = 0; i < it.count; i ++) {
            //TODO
            //Set a maximum height to check for collisions
            if(pos[i].y > 0 && pos[i].y < MAX_COLLIDE_HEIGHT) {
                if (CheckCollisionBoxes(
                        (BoundingBox){
                            (Vector3){  camera.position.x - 2,
                                        camera.position.y - 2,
                                        camera.position.z - 2 },
                            (Vector3){  camera.position.x + 2,
                                        camera.position.y + 2,
                                        camera.position.z + 2 }},
                        (BoundingBox){
                            (Vector3){  pos[i].x - shape[i].w/2,
                                        pos[i].y - shape[i].h/2,
                                        pos[i].z - shape[i].l/2 },
                            (Vector3){  pos[i].x + shape[i].w/2,
                                        pos[i].y + shape[i].h/2,
                                        pos[i].z + shape[i].l/2 }})) collision = true;
                
                if(collision) {
                    // printf("collide\n");
                    //Check the direction of the collision and only stop movement in that direction
                    // if(camera.position.x > pos[i].x && camera.position.x < pos[i].x + shape[i].w/2 && camera.position.x + 2 > pos[i].x + shape[i].w/2)
                    //     camera.position.x = camera.temp.x;
                    // if(camera.position.z > pos[i].z && camera.position.z < pos[i].z + shape[i].l/2 && camera.position.z + 2 > pos[i].z + shape[i].l/2)
                    //     camera.position.z = camera.temp.z;
                    camera.position = camera.temp;
                }
            }
        }
    }         
}

void CheckInteractions(ecs_iter_t* it) {
    Position *pos = ecs_term(it, Position, 1);
    Shape *shape = ecs_term(it, Shape, 2);
    Interaction *iaction = ecs_term(it, Interaction, 3);
    Scene *scene = ecs_term(it, Scene, 4);
    
    bool breakout = false;
    bool interacting;
    for(int i = 0; i < it->count; i++) {
        interacting = false;
        if(breakout) break;
        if(scene[i].id == gamescreen) {
            switch(iaction[i].type) {
                case 0: {
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if(shape[i]._3D) {
                            Ray ray = { 0 };                    // Picking line ray
                            RayCollision collision;
                            ray = GetMouseRay((Vector2){GetScreenWidth()/2, GetScreenHeight()/2}, camera);
                            collision = GetRayCollisionBox(ray,
                                        (BoundingBox){(Vector3){ pos[i].x - shape[i].w/2, pos[i].y - shape[i].h/2, pos[i].z - shape[i].l/2 },
                                        (Vector3){ pos[i].x + shape[i].w/2, pos[i].y + shape[i].h/2, pos[i].z + shape[i].l/2 }});
                            
                            if(collision.hit) {
                                //TODO
                                //interaction distance from camera to entity
                                if(camera.position.x > pos[i].x - 10 && camera.position.x < (pos[i].x + shape[i].w) + 10) {
                                    if(camera.position.z > pos[i].z - 10 && camera.position.z < (pos[i].z + shape[i].l) + 10) {
                                        interacting = collision.hit;
                                        // camera = sc->camera;
                                    }
                                }
                            }
                        }
                        else {
                            interacting = CheckCollisionRecs(
                                (Rectangle){GetMouseX(),GetMouseY(), 10, 10},
                                (Rectangle){pos[i].x, pos[i].y, shape[i].w, shape[i].h});
                        }
                        if(interacting) {
                            printf("Interacting!\n");
                            // printf("Has transition: %d\n", ecs_has_id(world, it->entities[i], ecs_id(Teleport)));
                            if(ecs_has_id(world, it->entities[i], ecs_id(Destroy))) {
                                ecs_delete(world, it->entities[i]);
                            }
                            else if(ecs_has_id(world, it->entities[i], ecs_id(Teleport))) {
                                const Teleport *tele = ecs_get_id(world, it->entities[i], ecs_id(Teleport));
                                // printf("Teleporting to %f %f %f!\n", tele->position.x, tele->position.y, tele->position.z);
                                camera.override = true;
                                camera.override_dirx = 1;
                                camera.override_diry = 0;
                                camera.position = (Vector3){tele->position.x, camera.position.y, tele->position.z};
                                breakout = true;
                            }
                        }
                    }
                    break;
                }
                case 1: {
                    break;
                }
                case 2: {
                    break;
                }
                default: break;
            }
        }
    }
}

void UnloadECS() {
    // ecs_query_t *mq = ecs_query_new(world, "Model_ECS");
    // ecs_iter_t itm = ecs_query_iter(mq);
    
    // while (ecs_query_next(&itm)) {
    //     Model_ECS *model = ecs_term(&itm, Model_ECS, 1);
    //     for (int i = 0; i < itm.count; i++) {
    //         UnloadModel(model[i].model);
    //     }
    // }
}

void DRAW_ECS(Camera *_camera) {
    ecs_query_t *q = ecs_query_new(world, "Position, Shape, Scene");
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        Position *pos = ecs_term(&it, Position, 1);
        Shape *shape = ecs_term(&it, Shape, 2);
        Scene *scene = ecs_term(&it, Scene, 3);
        
        for (int i = 0; i < it.count; i ++) {
            if(!ecs_has_id(world, it.entities[i], ecs_id(Model_ECS)) && !ecs_has_id(world, it.entities[i], ecs_id(Texture_ECS)) && scene[i].id == gamescreen) {
                // DrawRectangle(pos[i].x, pos[i].y, shape[i].w, shape[i].h, GRAY);

                // if(ecs_has_id(world, it.entities[i], ecs_id(SceneCamera))) {
                    // const SceneCamera *camera = ecs_get_id(world, it.entities[i], ecs_id(SceneCamera));
                Color color;
                if(ecs_has_id(world, it.entities[i], ecs_id(Color_ECS))) {
                    const Color_ECS *rgb = ecs_get_id(world, it.entities[i], ecs_id(Color_ECS));
                    color = (Color){rgb->r, rgb->g, rgb->b, 255};
                }
                else color = (Color){100,100,100, 255};

                if(shape[i]._3D) {
                    BeginMode3D(camera);
                    DrawCube((Vector3){pos[i].x, pos[i].y, pos[i].z}, shape[i].w, shape[i].h, shape[i].l, color);
                    DrawCubeWires((Vector3){pos[i].x, pos[i].y, pos[i].z}, shape[i].w, shape[i].h, shape[i].l, (Color){25, 25, 25, 255});
                    EndMode3D();
                }
                else DrawRectangle(pos[i].x, pos[i].y, shape[i].w, shape[i].h, color);

                if(scene[i].grid) {

                    BeginMode3D(camera);
                    DrawGrid(scene[i].width, 10);
                    EndMode3D();
                }

                if(ecs_has_id(world, it.entities[i], ecs_id(Text))) {  
                    const Text *text = ecs_get_id(world, it.entities[i], ecs_id(Text));
                    DrawText(text->text, pos[i].x, pos[i].y, text[i].size, WHITE);
                }
                // pos[i].y+=.5;
                // if(pos[i].y<=0) {
                //     pos[i].y = 0;
                // }
            }
        }
    }

    ecs_query_t *tq = ecs_query_new(world, "Position, Shape, Texture_ECS");
    ecs_iter_t itt = ecs_query_iter(tq);
    while (ecs_query_next(&itt)) {
        Position *pos = ecs_term(&itt, Position, 1);
        Shape *shape = ecs_term(&itt, Shape, 2);
        Texture_ECS *tex = ecs_term(&itt, Texture_ECS, 3);
        
        for (int i = 0; i < itt.count; i++) {
            BeginMode3D(camera);
            if(shape[i]._3D)
                DrawCubeTexture(tex[i].tex, (Vector3){ pos[i].x, pos[i].y, pos[i].z }, shape[i].w, shape[i].h, shape[i].l ,WHITE);
            else
                DrawBillboard(camera, tex[i].tex, (Vector3){ pos[i].x, pos[i].y, pos[i].z }, -shape[i].w, WHITE);
            
            EndMode3D();
        }
    }
    
    ecs_query_t *mq = ecs_query_new(world, "Position, Model_ECS");
    ecs_iter_t itm = ecs_query_iter(mq);
    
    while (ecs_query_next(&itm)) {
        Position *pos = ecs_term(&itm, Position, 1);
        Model_ECS *model = ecs_term(&itm, Model_ECS, 2);

        // printf("The file is %s\n", model->file_name);
        for (int i = 0; i < itm.count; i++) {
            // DrawModel(*model[i].model, (Vector3){ pos[i].x, pos[i].y, pos[i].z }, model[i].scale, WHITE);
            
            // rlScalef(model[i].scalef, 1.0f, 0.0f);
            // rlTranslatef(model[i].translatef, 1.0f, model[i].translatef);
            BeginMode3D(camera);
            if(ecs_has_id(world, itm.entities[i], ecs_id(Rotate))) {
                Rotate *rotate = ecs_get_id(world, itm.entities[i], ecs_id(Rotate));
                
                if(rotate->loop) {
                    rlRotatef(((rand()%20)/10), 0.0f, 1.0f, 0.0f);
                }
                else {
                    rlRotatef(rotate->angle, 0.0f, 1.0f, 0.0f);
                }
            }
            // model[i].rotatef+=((rand()%20)/10);
            // model[i].scalef+=((rand()%20)/10);
            // model[i].translatef+=((rand()%20)/10);
            DrawModelEx(*model[i].model, (Vector3){ pos[i].x, pos[i].y, pos[i].z }, (Vector3){ 0, 0, 0}, 0.0f, (Vector3){model[i].scale, model[i].scale, model[i].scale}, WHITE);
            EndMode3D();
        }
        
    }

    ecs_query_t *q2 = ecs_query_new(world, "Scene");
    ecs_iter_t it2 = ecs_query_iter(q2);
    while (ecs_query_next(&it2)) {
        Scene *scene = ecs_term(&it2, Scene, 1);   
        for (int i = 0; i < it2.count; i++) {
            if(scene[i].id == gamescreen) {
                if(scene[i].cursor) {
                    DrawRectangle(GetMouseX(), GetMouseY(), 10, 10, BLACK);
                }
                if(scene[i].fps) {
                    DrawFPS(10,10);
                }
            }
        }
    }
}
