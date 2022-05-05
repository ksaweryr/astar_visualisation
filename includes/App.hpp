#pragma once
#include <SDL2/SDL.h>
#include "Grid.hpp"
#include "Header.hpp"

class App {
private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    Grid* grid;
    Header* header;
    bool running;

    [[noreturn]] void error();

    void init();
    void handle_events();
    void draw();
public:
    App();
    ~App();

    void run();
};