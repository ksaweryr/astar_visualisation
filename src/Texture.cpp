#include "Texture.hpp"

Texture* Texture::create(SDL_Renderer* renderer, const void* mem, int data_size, std::pair<int, int> frame_size, int frame_duration) {
    SDL_RWops* rwops = SDL_RWFromConstMem(mem, data_size);

    if(rwops == nullptr) {
        return nullptr;
    }

    SDL_Surface* surf = SDL_LoadBMP_RW(rwops, SDL_TRUE);

    if(surf == nullptr) {
        return nullptr;
    }

    SDL_Texture* atlas = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_FreeSurface(surf);

    return atlas == nullptr ? nullptr : new Texture(atlas, frame_size, frame_duration);
}

Texture::Texture(SDL_Texture* atlas, std::pair<int, int> frame_size, int frame_duration) : frame_size(frame_size), frame_duration(frame_duration) {
    int w;

    SDL_QueryTexture(atlas, nullptr, nullptr, &w, nullptr);
    frame_count = w / frame_size.first;

    this->atlas = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>(atlas, SDL_DestroyTexture);
}

void Texture::draw(SDL_Renderer* renderer, SDL_Rect dst, double rotation_angle, uint8_t alpha) const {
    auto [w, h] = frame_size;
    uint64_t time = static_cast<uint64_t>(static_cast<double>(SDL_GetPerformanceCounter()) / SDL_GetPerformanceFrequency() * 1000);
    SDL_Rect src = {((time / frame_duration) % frame_count) * w, 0, w, h};

    SDL_SetTextureAlphaMod(atlas.get(), alpha);
    SDL_RenderCopyEx(renderer, atlas.get(), &src, &dst, rotation_angle, nullptr, SDL_FLIP_NONE);
}