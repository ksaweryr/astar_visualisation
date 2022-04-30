cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 17)

project(astar_visualisation)
file(GLOB SOURCES "src/*.cpp")

find_package(SDL2 REQUIRED)

add_custom_command(OUTPUT
    assets/ground.cpp
    assets/obstacle.cpp
    assets/path_border.cpp
    assets/path.cpp
    assets/start.cpp
    assets/target.cpp
    assets/visited.cpp
    assets/cell_overlay.cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND bash dump_assets.sh ${CMAKE_CURRENT_BINARY_DIR}/assets
    DEPENDS assets
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    assets/ground.cpp
    assets/obstacle.cpp
    assets/path_border.cpp
    assets/path.cpp
    assets/start.cpp
    assets/target.cpp
    assets/visited.cpp
    assets/cell_overlay.cpp
)
target_include_directories(${PROJECT_NAME}
    PRIVATE includes
    PRIVATE ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}
)