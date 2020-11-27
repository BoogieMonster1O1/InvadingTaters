cmake_minimum_required(VERSION 3.17)
project(SpaceInvaders)

add_executable(SpaceInvaders src/main.cpp)
set_property(TARGET SpaceInvaders PROPERTY CXX_STANDARD 14)
target_compile_options(SpaceInvaders PRIVATE -Wall)

# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(SpaceInvaders PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(SpaceInvaders PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(SpaceInvaders PRIVATE glm)
