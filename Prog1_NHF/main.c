#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Input.h"
#include "Timer.h"

#include <SDL2/SDL.h>
#include <stdio.h>

enum ComponentTypes
{
    Component_NONE = 0,
    Component_TRANSFORM = 1, //mat4
    Component_SPRITE = 2, //sprite
    Component_CAMERA = 3, //camera
    Component_PLAYER = 4, //player state 
};

typedef struct Sprite 
{
    vec4 tintColor;
    vec2 size;
    SubTexture subTex;
} Sprite;

typedef struct Camera 
{
    mat4 proj;
} Camera;

void RenderSprites(Renderer2D* renderer, Scene_t* scene) 
{
    //pre-create view to cache storages.
    View_t sprites = View_Create(scene, 2, Component_TRANSFORM, Component_SPRITE);

    int count = 0;

    //For each entity with a camera (usually only one)
    for (View_t cameras = View_Create(scene, 2, Component_CAMERA, Component_TRANSFORM); !View_End(&cameras); View_Next(&cameras)) 
    {
        Camera* cam = View_GetComponent(&cameras, 0);
        mat4* cam_tr = View_GetComponent(&cameras, 1);

        Renderer2D_BeginScene(renderer, mat4x4x4_Mul(cam->proj, mat4_Inverse(*cam_tr)));

        //Render each renderable.
        for (View_Reset(&sprites); !View_End(&sprites); View_Next(&sprites)) 
        {
            mat4* transform = View_GetComponent(&sprites, 0);
            Sprite* sprite = View_GetComponent(&sprites, 1);

            //Renderer2D_DrawSprite(renderer, *transform, sprite->size, sprite->tintColor, sprite->subTex);
            vec3 pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
            Renderer2D_DrawRotatedQuad_s(renderer, pos, sprite->size, 0.f, sprite->tintColor, sprite->subTex);
        }

        Renderer2D_EndScene(renderer);
        count++;
    }
}

typedef enum PlayerState
{
    Player_Idle,
    Player_Walking,
    Player_Attacking,
    Player_Dashing,
} PlayerState;

typedef struct PlayerComponent 
{
    PlayerState state;
    vec2 Direction;
    Timer_t timeSinceStateChange;
    Timer_t LastDash;
} PlayerComponent;

const float playerSpeed = 10.f;
const float dashTime = 0.2f;
const float dashSpeed = 40.f;
const float dashRecharge = 0.5f;

void UpdatePlayer(Scene_t* scene, float dt)
{
    View_t player = View_Create(scene, 3, Component_TRANSFORM, Component_SPRITE, Component_PLAYER);

    mat4* transform = View_GetComponent(&player, 0);
    Sprite* sprite = View_GetComponent(&player, 1);
    PlayerComponent* pc = View_GetComponent(&player, 2);

    InputSnapshot_t input = GetInput();

    vec2 inputDir = new_vec2_v(0.f);
    if (IsKeyPressed(&input, SDL_SCANCODE_W)) inputDir = vec2_Add(inputDir, new_vec2(0.f, 1.f));
    if (IsKeyPressed(&input, SDL_SCANCODE_S)) inputDir = vec2_Add(inputDir, new_vec2(0.f, -1.f));
    if (IsKeyPressed(&input, SDL_SCANCODE_D)) inputDir = vec2_Add(inputDir, new_vec2(1.f, 0.f));
    if (IsKeyPressed(&input, SDL_SCANCODE_A)) inputDir = vec2_Add(inputDir, new_vec2(-1.f, 0.f));
    inputDir = vec2_Normalize(inputDir);

    float Elapsed = GetElapsedSeconds(pc->timeSinceStateChange);

    switch (pc->state)
    {
    case Player_Idle:      
        if (vec2_Len(inputDir) > 0.f)
        {
            pc->timeSinceStateChange = MakeTimer();
            pc->state = Player_Walking;
        }
        else if (IsKeyPressed(&input, SDL_SCANCODE_LSHIFT) && GetElapsedSeconds(pc->LastDash) > dashRecharge)
        {
            pc->timeSinceStateChange = MakeTimer();
            pc->LastDash = MakeTimer();
            pc->state = Player_Dashing;
        }
        else 
        {
            *transform = mat4_Translate(*transform,
                new_vec3_v2(
                    vec2_Mul_s(pc->Direction, dt * max(0.f, playerSpeed * (0.2f - Elapsed)))
                    , 0.f)
            );
        }
        break;
    case Player_Walking:  
        if (vec2_Len(inputDir) < 0.1f)
        {
            pc->timeSinceStateChange = MakeTimer();
            pc->state = Player_Idle;
        }
        else if (IsKeyPressed(&input, SDL_SCANCODE_LSHIFT) && GetElapsedSeconds(pc->LastDash) > dashRecharge)
        {
            pc->timeSinceStateChange = MakeTimer();
            pc->LastDash = MakeTimer();
            pc->state = Player_Dashing;
        }
        else 
        {
            pc->Direction = inputDir;
            *transform = mat4_Translate(*transform,
                new_vec3_v2(
                    vec2_Mul_s(inputDir, dt * min(playerSpeed, playerSpeed * (Elapsed / 0.1f)))
                    , 0.f)
            );
        }
        break;
    case Player_Dashing:
        if (Elapsed > dashTime) 
        {
            pc->timeSinceStateChange = MakeTimer();
            pc->state = Player_Idle;
        }
        else 
        {
            *transform = mat4_Translate(*transform,
                new_vec3_v2(
                    vec2_Mul_s(pc->Direction, dt * dashSpeed)
                    , 0.f)
            );
        }
        break;
    }
}

int main(int argc, char* argv[])
{

    /* SDL inicializ�l�sa �s ablak megnyit�sa */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
    {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    SDL_Window* window = SDL_CreateWindow("SDL peldaprogram", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        880, 640,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);


    if (window == NULL)
    {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    
    //Init gl
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) exit(-1);

    //GLEnableDebugOutput();

    //Add components.
    Scene_t* scene = Scene_New();
    ComponentInfo_t transformInfo = COMPONENT_DEF(Component_TRANSFORM, mat4);
    Scene_AddComponentType(scene, transformInfo);
    ComponentInfo_t spriteInfo = COMPONENT_DEF(Component_SPRITE, Sprite);
    Scene_AddComponentType(scene, spriteInfo);
    ComponentInfo_t cameraInfo = COMPONENT_DEF(Component_CAMERA, Camera);
    Scene_AddComponentType(scene, cameraInfo);
    ComponentInfo_t playerInfo = COMPONENT_DEF(Component_PLAYER, PlayerComponent);
    Scene_AddComponentType(scene, playerInfo);


    Renderer2D renderer;
    Renderer2D_Init(&renderer);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);
    float aspect = (float)w / (float)h;

    entity_t e = Scene_CreateEntity(scene);
    mat4* tr = Scene_AddComponent(scene, e, Component_TRANSFORM);
    Sprite* s = Scene_AddComponent(scene, e, Component_SPRITE);
    PlayerComponent* pc = Scene_AddComponent(scene, e, Component_PLAYER);

    *tr = mat4x4_Identity(), new_vec3_v(-1.f);;
    s->subTex = SubTexture_empty();
    s->tintColor = new_vec4(0.f, 0.5f, 1.f, 1.f);
    s->size = new_vec2_v(1.f);
    pc->Direction = new_vec2_v(0.f);
    pc->state = Player_Idle;
    pc->timeSinceStateChange = MakeTimer();

    entity_t came = Scene_CreateEntity(scene);
    mat4* cam_tr = Scene_AddComponent(scene, came, Component_TRANSFORM);
    Camera* cam = Scene_AddComponent(scene, came, Component_CAMERA);
    *cam_tr = mat4x4_Identity();
    float scale = 10.f;
    cam->proj = mat4_Ortho(-aspect * scale, aspect * scale, scale, -scale, -1000, 1000);

    Timer_t timer = MakeTimer();

    SDL_Event ev;
    while (!(SDL_PollEvent(&ev) && ev.type == SDL_QUIT)) 
    {

        float timediff = GetElapsedSeconds(timer);
        timer = MakeTimer();

        UpdatePlayer(scene, timediff);

        Renderer2D_Clear(&renderer, new_vec4_v(0.f));

        RenderSprites(&renderer, scene);

        cam_tr = Scene_Get(scene, came, Component_TRANSFORM);
        *cam_tr = *(mat4*)Scene_Get(scene, e, Component_TRANSFORM);

        Renderer2D_BeginScene(&renderer, mat4x4x4_Mul(cam->proj, mat4_Inverse(*cam_tr)));
        
        vec2 camPos = new_vec2_v4(mat4x4_Mul_v(*cam_tr, new_vec4(0.f, 0.f, 0.f, 1.f)));
        for (float x = camPos.x - aspect * scale; x < camPos.x + aspect * scale; x++)
        {
            float x_ = floorf(x / 5.f) * 5.f;
            Renderer2D_DrawLine(&renderer,
                new_vec3(x_, camPos.y + scale, -1.f),
                new_vec3(x_, camPos.y - scale, -1.f),
                new_vec4_v(1.f)
            );
        }
        
        for (float y = camPos.y - scale; y < camPos.y + scale; y++)
        {
            float y_ = floorf(y / 5.f) * 5.f;
            Renderer2D_DrawLine(&renderer,
                new_vec3(camPos.x + aspect * scale, y_, -1.f),
                new_vec3(camPos.x - aspect * scale, y_, -1.f),
                new_vec4_v(1.f)
            );
        }
        
        Renderer2D_EndScene(&renderer);

        SDL_GL_SwapWindow(window);
        /* SDL_RenderPresent(renderer); - MacOS Mojave eset�n */

        SDL_PumpEvents();
    }

    Scene_Delete(scene);
    Renderer2D_Destroy(&renderer);
    SDL_GL_DeleteContext(glcontext);
    SDL_Quit();

    Tree_ResetPool();

    return 0;
}