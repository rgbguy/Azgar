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
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 800;
};

class Snake{
public:
    glm::vec2 Head = glm::vec2(0.0f,0.0f);
    std::vector<Block> Blocks;
    int numBlocks = 1;
};

Snake Azgar;
Block FoodBlock;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLFWwindow* Init();
void DrawSnakeBlock(Block block, unsigned int shaderProgram, unsigned int VAO);
void UpdateSnakeBlocks();
void DrawFoodBlock(unsigned int shaderProgram, unsigned int VAO);
void UpdateFoodBlock();
bool CheckGameOver();

int rightDir = 0;
int upDir = 0;
float updateDuration = 0.15f;
float spawnDuration = 2.0f;
// settings

std::chrono::time_point<std::chrono::system_clock> currentTime;

void RUN()
{
    UpdateFoodBlock();
    GLFWwindow* window = Init();
    glfwSetKeyCallback(window, key_callback);

    static GameState State = GameState();

    unsigned int shaderProgram = CreateShaderProgram("shaders/shader.vert", "shaders/shader.frag");
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
            LOG("FINAL SCORE: ", 0);
            LOG(Azgar.numBlocks-1, 0);
            LOG("\n", 0);
            glfwTerminate();
            return;
        }
        currentTime = std::chrono::system_clock::now();
        std::chrono::duration<double> resetElapsed = currentTime - resetTimer;        
        if(resetElapsed.count() > updateDuration)
        {
            resetTimer = std::chrono::system_clock::now();
            UpdateSnakeBlocks();
        }

        if(glm::length(10.0f * (FoodBlock.pos - Azgar.Head)) < 0.0001f)
        {
            UpdateFoodBlock();
            Azgar.numBlocks++;
            UpdateSnakeBlocks();
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "AZGAR game by RGBGuy", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    return window;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS && upDir == 0)
    {
        LOG("W pressed", 1);
        upDir = 1;
        rightDir = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS && rightDir == 0)
    {
        LOG("A pressed", 1);
        rightDir = -1;
        upDir = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS && upDir == 0)
    {
        LOG("S pressed", 1);
        upDir = -1;
        rightDir = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS && rightDir == 0)
    {
        LOG("D pressed", 1);
        rightDir = 1;
        upDir = 0;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        LOG("P pressed", 1);
        Azgar.numBlocks++;
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
        Azgar.Head += glm::vec2(rightDir*0.1f, upDir*0.1f);
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
        LOG("GAME OVER! You crashed into the border!\n", 0);
        return true;
    }
    for (int i = 1; i < Azgar.Blocks.size(); i++)
    {
        if(glm::length(10.0f * (Azgar.Blocks[i].pos - Azgar.Head)) < 0.01f)
        {
            LOG("GAME OVER! You ate yourself!\n", 0);
            return true;
        }
    }
    return false;
    
}
}