#include "App.hpp"
#include <iostream>
#include <SDL2/SDL_ttf.h>

App::App() {

}

App::~App() {
    if(grid != nullptr) {
        delete grid;
    }

    if(renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }

    if(window != nullptr) {
        SDL_DestroyWindow(window);
    }

    TTF_Quit();
    SDL_Quit();
}

void App::error() {
    std::cerr << "Unexpected error occured! " << SDL_GetError();
    std::exit(1);
}

void App::init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        error();
    }
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    window = SDL_CreateWindow("A* algorithm", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

    if(window == nullptr) {
        error();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(renderer == nullptr) {
        error();
    }

    if(TTF_Init() < 0) {
        error();
    }

    grid = Grid::create(renderer, 16, 10, {0, 100, 800, 600});

    if(grid == nullptr) {
        error();
    }

    header = Header::create(50, grid, {0, 0, 800, 100}, {255, 255, 255, 255});

    if(header == nullptr) {
        error();
    }

    running = true;
}

void App::handle_events() {
    SDL_Event e;

    while(SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_QUIT: {
                running = false;
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                int x = e.button.x;
                int y = e.button.y;
                if(encloses(grid->rect, {x, y})) {
                    grid->handle_mouse_click(e.button);
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                int x = e.motion.x;
                int y = e.motion.y;
                if(encloses(grid->rect, {x, y})) {
                    grid->handle_mouse_motion(e.motion);
                }
                break;
            }
            case SDL_MOUSEWHEEL: {
                grid->handle_mouse_wheel(e.wheel);
            }
        }
    }
}

void App::draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    grid->draw(renderer);
    header->draw(renderer);

    SDL_RenderPresent(renderer);
}

void App::run() {
    init();

    while(running) {
        handle_events();
        draw();
    }
}