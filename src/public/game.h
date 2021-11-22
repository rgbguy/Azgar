#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <vector>
#include "shader.h"
#include <thread> 
#include <chrono>
#include <ctime>
#include <random>

#define DEBUG 0
#define LOG(x, level) if(DEBUG || !level) std::cout << x

namespace SnakeGame
{
class Block{
public:
    glm::vec2 pos;
    int id;
    Block(int id_, glm::vec2 pos_) : id(id_), pos(pos_){}
    Block(){}
};

class GameState{
public:
    const unsigned int SCR_WIDTH = 500;
    const unsigned int SCR_HEIGHT = 500;

    int rightDir = 0;
    int upDir = 0;
    float updateDuration = 0.15f;
    bool paused = false;

    std::chrono::time_point<std::chrono::system_clock> currentTime;
};

class Snake{
public:
    glm::vec2 Head = glm::vec2(0.0f,0.0f);
    std::vector<Block> Blocks;
    int numBlocks = 1;
};

GLFWwindow* Init();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void DrawSnakeBlock(Block block, unsigned int shaderProgram, unsigned int VAO);
void UpdateSnakeBlocks();
void DrawFoodBlock(unsigned int shaderProgram, unsigned int VAO);
void UpdateFoodBlock();
bool CheckGameOver();
void PrintArt();
void PrintInstructions();
bool is_file_exist(const char *fileName);
std::string GetCurrentDirPath(const char * arg0);

Snake Azgar;
Block FoodBlock;
GameState gameState;

void RUN(const char * arg0)
{
    PrintArt();
    PrintInstructions();
    UpdateFoodBlock();
    GLFWwindow* window = Init();
    glfwSetKeyCallback(window, key_callback);

#ifdef __APPLE__
    std::string exepath = GetCurrentDirPath(arg0);
#else
    std::string exepath = "";
#endif
    std::string vspath = exepath + "shaders/shader.vert";
    std::string fspath = exepath + "shaders/shader.frag";

    unsigned int shaderProgram = CreateShaderProgram(vspath.c_str(), fspath.c_str());
    glUseProgram(shaderProgram);

    float vertices[] = {
         0.05f,  0.05f, 0.0f,  // top right
         0.05f, -0.05f, 0.0f,  // bottom right
        -0.05f, -0.05f, 0.0f,  // bottom left

        -0.05f, -0.05f, 0.0f,  // bottom left
        -0.05f,  0.05f, 0.0f,  // top left 
         0.05f,  0.05f, 0.0f   // top right
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    std::chrono::time_point<std::chrono::system_clock> resetTimer = std::chrono::system_clock::now();
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if(CheckGameOver())
        {
            LOG("---------------FINAL SCORE: ", 0);
            LOG(Azgar.numBlocks-1, 0);
            LOG("----------------\n", 0);
            glfwTerminate();
            return;
        }
        gameState.currentTime = std::chrono::system_clock::now();
        std::chrono::duration<double> resetElapsed = gameState.currentTime - resetTimer;        
        if(resetElapsed.count() > gameState.updateDuration)
        {
            resetTimer = std::chrono::system_clock::now();
            if(!gameState.paused) UpdateSnakeBlocks();
        }

        if(glm::length(10.0f * (FoodBlock.pos - Azgar.Head)) < 0.0001f)
        {
            UpdateFoodBlock();
            Azgar.numBlocks++;
        }

        for(int i = 0; i < Azgar.Blocks.size(); i++)
        {
            DrawSnakeBlock(Azgar.Blocks[i], shaderProgram, VAO);
        }
        DrawFoodBlock(shaderProgram, VAO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

}

GLFWwindow* Init()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(gameState.SCR_WIDTH, gameState.SCR_HEIGHT, "AZGAR game by RGBGuy", NULL, NULL);
    if (window == NULL)
    {
        LOG("Failed to create GLFW window\n", 0);
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG("Failed to initialize GLAD\n", 0);
    }
    return window;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS && gameState.upDir == 0)
    {
        LOG("W pressed", 1);
        gameState.upDir = 1;
        gameState.rightDir = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS && gameState.rightDir == 0)
    {
        LOG("A pressed", 1);
        gameState.rightDir = -1;
        gameState.upDir = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS && gameState.upDir == 0)
    {
        LOG("S pressed", 1);
        gameState.upDir = -1;
        gameState.rightDir = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS && gameState.rightDir == 0)
    {
        LOG("D pressed", 1);
        gameState.rightDir = 1;
        gameState.upDir = 0;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        LOG("P pressed", 1);
        gameState.paused = !gameState.paused;
        if(gameState.paused) { LOG("---------------GAME PAUSED-------------------\n", 0); }
        else { LOG("---------------GAME UNPAUSED-----------------\n", 0); }

    } 
}

void DrawSnakeBlock(Block block, unsigned int shaderProgram, unsigned int VAO)
{
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(block.pos, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMat"), 1, GL_FALSE, &modelMat[0][0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "SnakeBlockID"), float(block.id));
    glUniform1f(glGetUniformLocation(shaderProgram, "numBlocks"), float(Azgar.numBlocks));
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 6); 
}

void DrawFoodBlock(unsigned int shaderProgram, unsigned int VAO)
{
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(FoodBlock.pos,0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMat"), 1, GL_FALSE, &modelMat[0][0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "SnakeBlockID"), 0.0f);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 6); 
}

void UpdateSnakeBlocks()
{
    for(int i = Azgar.Blocks.size()-1; i > 0; i--)
    {
        Azgar.Blocks[i].pos = Azgar.Blocks[i-1].pos;
    }

    for (int i = 0; i < Azgar.numBlocks - Azgar.Blocks.size(); i++)
    {
        Azgar.Blocks.push_back(Block(Azgar.numBlocks, Azgar.Head));
    }

    Azgar.Head += glm::vec2(gameState.rightDir*0.1f, gameState.upDir*0.1f);
    Azgar.Blocks[0].pos = Azgar.Head;
}

void UpdateFoodBlock()
{
    int max = 9;
    int min = -9    ;
    int randNumX = rand()%(max-min + 1) + min;
    int randNumY = rand()%(max-min + 1) + min;
   
    FoodBlock.pos = glm::vec2(randNumX*0.1f, randNumY*0.1f);
}

bool CheckGameOver()
{
    if(abs(Azgar.Head.x) > 1.0f || abs(Azgar.Head.y) > 1.0f)
    {
        LOG("---------------***********-------------------\n", 0);
        LOG("----GAME OVER! Crashed into the border!------\n", 0);
        LOG("---------------***********-------------------\n", 0);
        return true;
    }
    for (int i = 1; i < Azgar.Blocks.size(); i++)
    {
        if(glm::length(10.0f * (Azgar.Blocks[i].pos - Azgar.Head)) < 0.01f)
        {
            LOG("---------------***********-------------------\n", 0);
            LOG("--------GAME OVER! You ate yourself!---------\n", 0);
            LOG("---------------***********-------------------\n", 0);
            return true;
        }
    }
    return false;
}
void PrintArt(){
    std::cout << R"(

____ ___  ____ ____ ____       
|__|   /  | __ |__| |__/       
|  |  /__ |__] |  | |  \       
                               
___  _   _                     
|__]  \_/                      
|__]   |                       
                               
____ ____ ___  ____ _  _ _   _ 
|__/ | __ |__] | __ |  |  \_/  
|  \ |__] |__] |__] |__|   |   
                               
                                             
)" << '\n';

}

void PrintInstructions()
{
    std::cout << "INSTRUCTIONS:\n A-S-W-D to move.\n P to Pause\n Try not to die!\n";
}

bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

#ifdef __APPLE__
std::string GetCurrentDirPath(const char * arg0)
{
    char *resolved = realpath(arg0, NULL);
    if (resolved != NULL) {
        char *fname = strrchr(resolved, '/');
        if (fname != NULL) {
            fname[1] = '\0';
        }
        std::string path = std::string(resolved);
        free(resolved);
        return path;
    } else {
        perror("realpath");
        return "";
    }
}
#endif
}

