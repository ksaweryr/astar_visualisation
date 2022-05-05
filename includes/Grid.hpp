#pragma once
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <SDL2/SDL.h>
#include "Texture.hpp"
#include "utils.hpp"

class Grid {
private:
    enum class Tex {
        GROUND,
        OBSTACLE,
        START,
        TARGET,
        VISITED,
        PATH,
        PATH_BORDER,
        CELL_OVERLAY,
        __COUNT
    };

    using Cell = std::pair<size_t, size_t>;

    inline static std::vector<Tex> placeable{Tex::OBSTACLE, Tex::START, Tex::TARGET};

    size_t _width;
    size_t _height;
    int _tile_size;
    SDL_Rect _rect;
    uint64_t _last_update{};
    Cell hover{};
    int selected_tile{};
    Cell start;
    Cell target;
    std::set<Cell> obstacles{};
    std::set<Cell> visited{};
    std::vector<Cell> path{};

    std::vector<std::unique_ptr<Texture>> textures;

    Grid(size_t width, size_t height, SDL_Rect dst, std::vector<std::unique_ptr<Texture>>&& textures);

    double heuristic(Cell cell) const;

    void draw_tile(SDL_Renderer* renderer, Tex texture, Cell cell, double rotation=0, uint8_t alpha=255) const;
public:
    size_t const& width{_width};
    size_t const& height{_height};
    int const& tile_size{_tile_size};
    SDL_Rect const& rect{_rect};
    uint64_t const& last_update{_last_update};
    static Grid* create(SDL_Renderer* renderer, size_t width, size_t height, SDL_Rect dst);

    void draw(SDL_Renderer* renderer) const;
    void update();
    void handle_mouse_motion(SDL_MouseMotionEvent const& e);
    void handle_mouse_click(SDL_MouseButtonEvent const& e);
    void handle_mouse_wheel(SDL_MouseWheelEvent const& e);

    int get_path_len() const;
    int get_visited_tiles_count() const;

    void set_start(size_t x, size_t y);
    void set_target(size_t x, size_t y);
    void toggle_obstacle(size_t x, size_t y);
};