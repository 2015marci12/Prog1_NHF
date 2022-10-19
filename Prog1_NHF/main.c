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
    Component_TRANSFORM, //mat4
    Component_SPRITE, //sprite
    Component_CAMERA, //camera
    Component_PLAYER, //player state 
    Component_PLANE,
};

typedef struct Sprite 
{
    vec4 tintColor;
    vec2 size;
    SubTexture subTex;
    SubTexture overlays[5];
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

            Renderer2D_DrawSprite(renderer, *transform, sprite->size, sprite->tintColor, sprite->subTex);
            for (int i = 0; i < 5; i++) 
            {
                if (sprite->overlays[i].texture != NULL) 
                {
                    mat4 tr = mat4_Translate(*transform, new_vec3(0.f, 0.f, i * 0.01f));
                    Renderer2D_DrawSprite(renderer, tr, sprite->size, sprite->tintColor, sprite->overlays[i]);
                }
            }
        }

        Renderer2D_EndScene(renderer);
        count++;
    }
}

typedef struct PlayerComponent 
{
    float fuel;
} PlayerComponent;

typedef struct PlaneMovementComponent 
{
    vec2 velocity;
    float liftcoeff;
    float dragcoeff;
    float thrust;
    float mass;
} PlaneMovementComponent;

const float g = 5.f;
const float thrust_idle = 13.f;
const float thrust_booster = 25.f;
const float lift_coeff = 1.1f;
const float drag_coeff = 0.1f;
const float plane_mass = 1.f;
const float booster_fuelconsumption = 0.1f;

void MovePlanes(Scene_t* scene, float dt, Renderer2D* renderer)
{
    for (View_t view = View_Create(scene, 2, Component_TRANSFORM, Component_PLANE); !View_End(&view); View_Next(&view)) 
    {
        mat4* transform = View_GetComponent(&view, 0);
        PlaneMovementComponent* plane = View_GetComponent(&view, 1);

        vec2 forward = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(1.f, 0.f, 0.f, 1.f)));
        vec3 pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
        vec2 direction = vec2_Normalize(vec2_Sub(forward, new_vec2_v3(pos)));
        vec2 velocitynormal = vec2_Normalize(plane->velocity);
        float vel = vec2_Len(plane->velocity);

        float dirAngle = vec2_Angle(direction);
        float velAngle = vec2_Angle(velocitynormal);

        //Angle differernce.
        float anglediff = dirAngle - velAngle;
        anglediff += (anglediff > PI) ? -(PI * 2.f) : (anglediff < -PI) ? (PI * 2.f) : 0;
        float aoa = max(-1.f, min(1.f, (anglediff))); //aoa restricted between [-1, 1]

        vec2 aeroforce = vec2_Mul_s(velocitynormal, -1.f * (0.5f + fabsf(aoa)) * plane->dragcoeff * (vel * vel)); //drag.
        vec2 thrustforce = vec2_Mul_s(direction, plane->thrust); //thrust.
        vec2 gravity = vec2_Mul_s(new_vec2(0.f, -1.f), g * plane->mass); //gravity.

        //Lift.
        vec2 lift = vec2_Rot(new_vec2(0.f, vel * aoa * plane->liftcoeff), velAngle);
        //Adjust for straight flying.
        lift = vec2_Add(lift, new_vec2(0.f, fabsf(plane->velocity.x) / 5.f * plane->liftcoeff));

        //Sum forces and divide by mass.
        vec2 accel = vec2_Div_s(vec2_Add(vec2_Add(vec2_Add(aeroforce, thrustforce), gravity), lift), plane->mass);

        //move.
        *transform = mat4_Rotate(mat4_Translate(mat4x4_Identity(), vec3_Add(pos, new_vec3_v2(vec2_Mul_s(plane->velocity, dt), 0.f))), dirAngle, new_vec3(0.f, 0.f, 1.f));
        
        //Apply acceleration.
        plane->velocity = vec2_Add(plane->velocity, vec2_Mul_s(accel, dt));

        if (fabsf(velAngle) > PI / 2.f) *transform = mat4_Scale(*transform, new_vec3(1.f, -1.f, 1.f)); //Invert sprite if the velocity is facing the other way.
    }
}

SDL_Window* window;

GLTexture* tex;
TextureAtlas atlas;
SubTexture planeTex;
Animation boosterAnim;
Animation cannonAnim;

void UpdatePlayer(Scene_t* scene, float dt)
{
    View_t view = View_Create(scene, 4, Component_TRANSFORM, Component_PLAYER, Component_PLANE, Component_SPRITE);
    InputSnapshot_t input = GetInput();

    mat4* transform = View_GetComponent(&view, 0);
    PlaneMovementComponent* pm = View_GetComponent(&view, 2);
    Sprite* sprite = View_GetComponent(&view, 3);

    vec3 Pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
    ivec2 screenSize;
    SDL_GetWindowSize(window, &screenSize.x, &screenSize.y);
    ivec2 mousePos = GetMousePos(&input);
    ivec2 screenCenter = ivec2_Div_s(screenSize, 2);
    vec2 mouseDiff = ivec2_to_vec2(ivec2_Sub(mousePos, screenCenter));
    mouseDiff.y *= -1.f;

    //Look at mouse.
    float angle = vec2_Angle(mouseDiff);
    *transform = mat4_Rotate(mat4_Translate(mat4x4_Identity(), Pos), angle, new_vec3(0.f, 0.f, 1.f)); 

    //scale thrust.
    float thrust = thrust_idle;
    thrust *= max(0.f, min(vec2_Len(mouseDiff) / (float)ivec2_Min(screenCenter), 1.f));
    if (IsKeyPressed(&input, SDL_SCANCODE_SPACE)) 
    {
        thrust = thrust_booster;
        //Booster animation.
        Timer_t timer = { 0 };
        sprite->overlays[0] = Animation_GetAt(&boosterAnim, GetElapsedSeconds(timer), NULL);
    }
    else 
    {
        sprite->overlays[0] = SubTexture_empty();
    }
    pm->thrust = thrust;

    //shooting.
    if (IsMouseButtonPressed(&input, SDL_BUTTON_LEFT)) 
    {
        //cannon animation.
        Timer_t timer = { 0 };
        sprite->overlays[1] = Animation_GetAt(&cannonAnim, GetElapsedSeconds(timer), NULL);
    }
    else 
    {
        sprite->overlays[1] = SubTexture_empty();
    }
}

int main(int argc, char* argv[])
{

    /* SDL inicializálása és ablak megnyitása */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
    {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    window = SDL_CreateWindow("SDL peldaprogram", 
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

    tex = LoadTex2D("jet.png");
    atlas = TextureAtlas_create(tex, new_uvec2(64, 32));
    planeTex = TextureAtlas_SubTexture(&atlas, new_uvec2(0, 3), new_uvec2(1, 1));

    boosterAnim.frameCount = 3;
    boosterAnim.frameTime = 0.16f;
    boosterAnim.frames[0] = TextureAtlas_SubTexture(&atlas, new_uvec2(0, 1), new_uvec2(1, 1));
    boosterAnim.frames[1] = TextureAtlas_SubTexture(&atlas, new_uvec2(0, 0), new_uvec2(1, 1));
    boosterAnim.frames[2] = TextureAtlas_SubTexture(&atlas, new_uvec2(1, 1), new_uvec2(1, 1));

    cannonAnim.frameCount = 3;
    cannonAnim.frameTime = 0.08f;
    cannonAnim.frames[0] = TextureAtlas_SubTexture(&atlas, new_uvec2(0, 2), new_uvec2(1, 1));
    cannonAnim.frames[2] = TextureAtlas_SubTexture(&atlas, new_uvec2(1, 3), new_uvec2(1, 1));
    cannonAnim.frames[3] = SubTexture_empty();

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
    ComponentInfo_t planeInfo = COMPONENT_DEF(Component_PLANE, PlaneMovementComponent);
    Scene_AddComponentType(scene, planeInfo);


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
    PlaneMovementComponent* pm = Scene_AddComponent(scene, e, Component_PLANE);

    *tr = mat4x4_Identity(), new_vec3_v(-1.f);;
    s->subTex = planeTex;
    s->tintColor = new_vec4_v(1.f);
    s->size = new_vec2(2.f, 1.0f);
    for(int i = 0; i < 5; i++) s->overlays[i] = SubTexture_empty();
    PlaneMovementComponent temp = { new_vec2_v(0.f), lift_coeff, drag_coeff, thrust_booster, plane_mass };
    *pm = temp;

    entity_t came = Scene_CreateEntity(scene);
    mat4* cam_tr = Scene_AddComponent(scene, came, Component_TRANSFORM);
    Camera* cam = Scene_AddComponent(scene, came, Component_CAMERA);
    *cam_tr = mat4x4_Identity();
    float scale = 10.f;
    cam->proj = mat4_Ortho(-aspect * scale, aspect * scale, scale, -scale, -1000, 1000);

    Timer_t timer = MakeTimer();

    SDL_Event ev;
    bool exit = false;
    while (!exit) 
    {
        while (SDL_PollEvent(&ev)) 
        {
            exit |= ev.type == SDL_QUIT;
        }

        float timediff = GetElapsedSeconds(timer);
        timer = MakeTimer();

        Renderer2D_Clear(&renderer, new_vec4_v(0.f));

        UpdatePlayer(scene, timediff);
        MovePlanes(scene, timediff, &renderer);

        RenderSprites(&renderer, scene);

        cam_tr = Scene_Get(scene, came, Component_TRANSFORM);
        tr = Scene_Get(scene, e, Component_TRANSFORM);
        vec3 Pos = new_vec3_v4(mat4x4_Mul_v(*tr, new_vec4(0.f, 0.f, 0.f, 1.f)));
        *cam_tr = mat4_Translate(mat4x4_Identity(), Pos);

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
        /* SDL_RenderPresent(renderer); - MacOS Mojave esetén */

        SDL_PumpEvents();
    }

    Scene_Delete(scene);
    GLTexture_Destroy(tex);
    Renderer2D_Destroy(&renderer);
    SDL_GL_DeleteContext(glcontext);
    SDL_Quit();

    Tree_ResetPool();

    return 0;
}