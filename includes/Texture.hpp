#pragma once
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <SDL2/SDL.h>

class Texture {
private:
    std::pair<int, int> frame_size;
    int frame_count;
    int frame_duration; // in ms
    std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>> atlas;

    Texture(SDL_Texture* atlas, std::pair<int, int> frame_size, int frame_duration);
public:
    static Texture* create(SDL_Renderer* renderer, const void* mem, int data_size, std::pair<int, int> frame_size, int frame_duration);

    void draw(SDL_Renderer* renderer, SDL_Rect dst, double rotation_angle, uint8_t alpha);
};