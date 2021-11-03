cmake_minimum_required(VERSION 3.21.3)

project(Azgar)

add_executable(${PROJECT_NAME}
private/Main.cpp
private/Shader.cpp)

find_package(OpenGL REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ext/glfw ${CMAKE_CURRENT_SOURCE_DIR}/../build/ext/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ext/glad ${CMAKE_CURRENT_SOURCE_DIR}/../build/ext/glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ext/glm ${CMAKE_CURRENT_SOURCE_DIR}/../build/ext/glm)

target_include_directories(${PROJECT_NAME}
PUBLIC ../ext/glfw/include
PUBLIC ../ext/glad/include
PUBLIC ../ext/glm/glm)

target_link_directories(${PROJECT_NAME}
PUBLIC ../ext/glfw/src
PUBLIC ../ext/glad/src
PUBLIC ../ext/glm/glm)

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw glad glm)


