#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>

enum ComponentTypes
{
    Component_NONE = 0,
    Component_TRANSFORM = 1,
};

typedef struct Transform 
{
    mat4 transform;
} Transform;

int main(int argc, char* argv[])
{

    /* SDL inicializálása és ablak megnyitása */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
    {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("SDL peldaprogram", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        440, 360,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window == NULL)
    {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    
    //Init gl
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) exit(-1);

    GLEnableDebugOutput();

    Scene_t* scene = Scene_New();
    ComponentInfo_t transformInfo = COMPONENT_DEF(Component_TRANSFORM, Transform);
    Scene_AddComponentType(scene, transformInfo);

    for (int i = 0; i < 100; i++) 
    {
        entity_t e = Scene_CreateEntity(scene);
        Transform* tr = Scene_AddComponent(scene, e, Component_TRANSFORM);
        mat4 t;
        for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) t.col[i].comp[j] = !!j == i;
        tr->transform = t;

        ASSERT(tr == Scene_Get(scene, e, Component_TRANSFORM));
    }
    TRACE("%s\n", Scene_GetStorage(scene, Component_TRANSFORM)->comp.name);
    for (View_t v = View_Create(scene, 1, Component_TRANSFORM); !View_End(&v); View_Next(&v)) 
    {
        Transform* tr = View_GetComponent(&v, 0);
        ASSERT(tr);
    }
    Scene_Delete(scene);

    glViewport(0, 0, 440, 360);

    glClearColor(1.f, 0.5f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };
    GLBuffer* vbo = GLBuffer_Create(9 * sizeof(float), 0, vertices);

    vertexAttribute_t attribs[] = {
        { 0, 0, GLDataType_FLOAT, 3, false, 0}
    };
    GLVertexArray* vao = GLVertexArray_Create(attribs, 1);
    GLVertexArray_BindBuffer(vao, 0, vbo, 0, 3 * sizeof(float), GLAttribDiv_PERVERTEX);
    GLVertexArray_Bind(vao);

    const char vertexsource[] =
        "   #version 330 core                                       \n"
        "   layout(location = 0) in vec3 aPos;                      \n"
        "                                                           \n"
        "   void main()                                             \n"
        "   {                                                       \n"
        "       gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);    \n"
        "   }                                                       \n"
        ;
    const char fragmentsource[] =
        "   #version 330 core                                       \n"
        "   out vec4 FragColor;                                     \n"
        "                                                           \n"
        "   void main()                                             \n"
        "   {                                                       \n"
        "       FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);           \n"
        "   }                                                       \n"
        ;
    shaderSource_t sources[] =
    {
        { GlShaderType_VERT, vertexsource },
        { GlShaderType_FRAG, fragmentsource },
    };
    GLShader* shader = GLShader_Create(sources, 2);
    GLShader_Bind(shader);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(window);

    GLBuffer_Destroy(vbo);
    GLVertexArray_Destroy(vao);
    GLShader_Destroy(shader);

    /* varunk a kilepesre */
    SDL_Event ev;
    while (SDL_WaitEvent(&ev) && ev.type != SDL_QUIT) {
        /* SDL_RenderPresent(renderer); - MacOS Mojave esetén */
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_Quit();

    Tree_ResetPool();

    return 0;
}