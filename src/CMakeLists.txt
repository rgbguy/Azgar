cmake_minimum_required(VERSION 3.21.3)

project(Azgar)

add_executable(${PROJECT_NAME} private/main.cpp)

find_package(OpenGL REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ext/glfw ${CMAKE_CURRENT_SOURCE_DIR}/../build/ext/glfw)

target_include_directories(${PROJECT_NAME} PUBLIC ../ext/glfw/include)
target_link_directories(${PROJECT_NAME} PUBLIC ../ext/glfw/src)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw)


