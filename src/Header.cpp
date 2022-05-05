#include "Header.hpp"
#include <algorithm>
#include <sstream>

namespace FontData {
    extern "C" unsigned char font[];
    extern "C" unsigned int font_len;
}

Header::Header(const Grid* grid, TTF_Font* font, SDL_Rect rect, SDL_Color color) : grid(grid), rect(rect), color(color) {
    this->font = std::unique_ptr<TTF_Font, std::function<void(TTF_Font*)>>(font, TTF_CloseFont);
}

Header* Header::create(int font_size, const Grid* grid, SDL_Rect rect, SDL_Color color) {
    SDL_RWops* rwops = SDL_RWFromMem(FontData::font, FontData::font_len);

    if(rwops == nullptr) {
        return nullptr;
    }

    TTF_Font* font = TTF_OpenFontRW(rwops, SDL_TRUE, font_size);

    return font == nullptr ? nullptr : new Header(grid, font, rect, color);
}

void Header::draw(SDL_Renderer* renderer) {
    if(last_update <= grid->last_update || !tex) {
        update(renderer);
    }

    int w, h;
    double scale;
    SDL_Rect target_rect;

    SDL_QueryTexture(tex.get(), nullptr, nullptr, &w, &h);
    scale = std::min(static_cast<double>(rect.w) / w, static_cast<double>(rect.h) / h);
    target_rect = {rect.x, rect.y, static_cast<int>(w * scale), static_cast<int>(h * scale)};

    SDL_RenderCopy(renderer, tex.get(), nullptr, &target_rect);
}

void Header::update(SDL_Renderer* renderer) {
    std::stringstream buf{};
    std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>> path_len_tex{};
    std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>> visited_vertices_tex{};
    int w1, w2, h1, h2;
    SDL_Rect target_rect;

    buf << "Długość ścieżki: " << grid->get_path_len();
    path_len_tex = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>(
        create_text_texture(renderer, buf.str().c_str(), color),
        SDL_DestroyTexture
    );
    buf.str("");

    buf << "Odwiedzone wierzchołki: " << grid->get_visited_tiles_count() << '/' << (grid->width * grid->height);
    visited_vertices_tex = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>(
        create_text_texture(renderer, buf.str().c_str(), color),
        SDL_DestroyTexture
    );

    SDL_QueryTexture(path_len_tex.get(), nullptr, nullptr, &w1, &h1);
    SDL_QueryTexture(visited_vertices_tex.get(), nullptr, nullptr, &w2, &h2);
    tex = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>(
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, std::max(w1, w2), h1 + h2),
        SDL_DestroyTexture
    );
    
    SDL_SetRenderTarget(renderer, tex.get());
    SDL_RenderClear(renderer);

    target_rect = {0, 0, w1, h1};
    SDL_RenderCopy(renderer, path_len_tex.get(), nullptr, &target_rect);

    target_rect = {0, h1, w2, h2};
    SDL_RenderCopy(renderer, visited_vertices_tex.get(), nullptr, &target_rect);

    SDL_SetRenderTarget(renderer, nullptr);
    last_update = SDL_GetPerformanceCounter();
}

SDL_Texture* Header::create_text_texture(SDL_Renderer* renderer, const char* text, SDL_Color color) const {
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font.get(), text, color);

    if(surf == nullptr) {
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    
    return tex;
}