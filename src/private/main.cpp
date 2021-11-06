#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <loguru.cpp>
#include <vector>
#include "shader.h"
#include <thread> 
#include <chrono>
#include <ctime>

class SnakeBlock{
public:
    glm::vec2 pos;
    int id;
    SnakeBlock(int id_, glm::vec2 pos_)
    {
        id = id_;
        pos = pos_;
    }
};

glm::vec2 SnakeHead;
std::vector<SnakeBlock> SnakeBlocks;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLFWwindow* Init();

int rightDir = 0;
int upDir = 0;
float speed = 0.001f;
// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char* argv[])
{
    SnakeHead = glm::vec2(0.0f,0.0f);
    for (int i = 0; i < 5; i++)
    {
        SnakeBlocks.push_back(SnakeBlock(i, float(i) * glm::vec2(-0.1f, 0.0f) + SnakeHead));
    }
    


    GLFWwindow* window = Init();
    glfwSetKeyCallback(window, key_callback);

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


    // Get a handle for our "MVP" uniform
    // Only during the initialisation
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "modelMat");
    float SnakeBlockID = glGetUniformLocation(shaderProgram, "SnakeBlockID");


    glm::mat4 modelMat = glm::mat4(1.0f);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &modelMat[0][0]);

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)


    // Using time point and system_clock
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
            for(auto it: SnakeBlocks)
    {
        std::cout << "Block "<< it.id << ": " << it.pos.x << "," << it.pos.y << "\n";
    }
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;        
        if(elapsed_seconds.count() > 0.1f)
        {
            std::cout << "Reset!\n";
            start = std::chrono::system_clock::now();
            for(int i = 4; i > 0; i--)
            {
                SnakeBlocks[i].pos = SnakeBlocks[i-1].pos;
            }
            SnakeHead += glm::vec2(rightDir*0.1f, upDir*0.1f);

            SnakeBlocks[0].pos = SnakeHead;
        }

        for(int i = 0; i < 5; i++)
        {
            modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(SnakeBlocks[i].pos, 0.0f));
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &modelMat[0][0]);
            glUniform1f(SnakeBlockID, float(SnakeBlocks[i].id));
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            glDrawArrays(GL_TRIANGLES, 0, 6); 
        }
  
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

GLFWwindow* Init()
{
    // Put every log message in "debug.log":
    loguru::add_file("debug.log", loguru::Append, loguru::Verbosity_MAX);

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    LOG_F(INFO, "OpenGL version is %s" , glGetString(GL_VERSION));
    return window;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        std::cout << "W Pressed\n";
        upDir = 1;
        rightDir = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        std::cout << "A Pressed\n";
        rightDir = -1;
        upDir = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        std::cout << "S Pressed\n";
        upDir = -1;
        rightDir = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        std::cout << "D Pressed\n";
        rightDir = 1;
        upDir = 0;
    } 
}