#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>

int keysTest[60] =
{
    39, 45, 69, 83, 89, 93, 153, 172, 214, 222, 248, 257, 258, 265, 266, 273, 312, 314, 331, 334, 336, 392, 428, 447, 450, 462, 506, 527, 529, 531, 565, 575, 578, 598, 615, 622, 627, 628, 669, 693, 764, 770, 778, 792, 796, 801, 822, 835, 845, 866, 887, 889, 907, 909, 938, 942, 945, 960, 965, 993
};

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

    TreeNode_t* root = NULL;
    for (int i = 0; i < 60; i++) 
    {
        root = Tree_Insert(root, keysTest[i], keysTest[i]);
        int height;
        ASSERT(Tree_TestCheckCachedHeights(root, &height), "Tree heights invalid!");
    }

    for(int i = 0; i < 60; i++) 
    {
        root = Tree_Remove(root, keysTest[i]);
        int height;
        ASSERT(Tree_TestCheckCachedHeights(root, &height), "Tree heights invalid!");
    }
    
    //Init gl
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) exit(-1);

    GLEnableDebugOutput();

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