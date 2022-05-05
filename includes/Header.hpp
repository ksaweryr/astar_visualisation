#pragma once
#include <functional>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Grid.hpp"

class Header {
private:
    const Grid* grid;
    std::unique_ptr<TTF_Font, std::function<void(TTF_Font*)>> font;
    std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>> tex{};
    SDL_Rect rect;
    SDL_Color color;
    uint64_t last_update{};

    Header(const Grid* grid, TTF_Font* font, SDL_Rect rect, SDL_Color color);

    void update(SDL_Renderer* renderer);
public:
    static Header* create(int font_size, const Grid* grid, SDL_Rect rect, SDL_Color color);

    void draw(SDL_Renderer* renderer);
    SDL_Texture* create_text_texture(SDL_Renderer* renderer, const char* text, SDL_Color color) const;
};