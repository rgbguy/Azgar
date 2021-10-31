cmake_minimum_required(VERSION 3.21.3)

project(Azgar)

add_executable(${PROJECT_NAME} private/main.cpp)

find_package(OpenGL REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ext/glfw ${CMAKE_CURRENT_SOURCE_DIR}/../build/ext/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ext/glad ${CMAKE_CURRENT_SOURCE_DIR}/../build/ext/glad)

target_include_directories(${PROJECT_NAME}
PUBLIC ../ext/glfw/include
PUBLIC ../ext/glad/include)

target_link_directories(${PROJECT_NAME}
PUBLIC ../ext/glfw/src
PUBLIC ../ext/glad/src)

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw glad)


