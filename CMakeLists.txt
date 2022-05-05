cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 17)

project(astar_visualisation)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

file(GLOB SOURCES "src/*.cpp")

file(GLOB ASSETS "assets/**/*")
list(FILTER ASSETS EXCLUDE REGEX "txt$")
list(TRANSFORM ASSETS REPLACE "^.*/(assets/.*)$" "\\1")

set(DUMPED_ASSETS ${ASSETS})
list(TRANSFORM DUMPED_ASSETS REPLACE "^.*/(.*)\\..*$" "assets/\\1.cpp")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_custom_command(OUTPUT assets
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND mkdir -p assets
)

foreach(source target IN ZIP_LISTS ASSETS DUMPED_ASSETS)
    add_custom_command(OUTPUT ${target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND bash dump_assets.sh ${source} ${CMAKE_CURRENT_BINARY_DIR}/assets
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/assets
        DEPENDS ${source}
    )
endforeach()

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${DUMPED_ASSETS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE includes
    PRIVATE ${SDL2_INCLUDE_DIRS}
    PRIVATE ${SDL2_TTF_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARY}
)