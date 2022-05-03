#include "Grid.hpp"
#include <SDL2/SDL_assert.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

constexpr int INF = std::numeric_limits<int>::max();

namespace TexData {
    extern "C" unsigned char ground[];
    extern "C" unsigned int ground_len;

    extern "C" unsigned char obstacle[];
    extern "C" unsigned int obstacle_len;

    extern "C" unsigned char start[];
    extern "C" unsigned int start_len;

    extern "C" unsigned char target[];
    extern "C" unsigned int target_len;

    extern "C" unsigned char visited[];
    extern "C" unsigned int visited_len;

    extern "C" unsigned char path[];
    extern "C" unsigned int path_len;

    extern "C" unsigned char path_border[];
    extern "C" unsigned int path_border_len;

    extern "C" unsigned char cell_overlay[];
    extern "C" unsigned int cell_overlay_len;
}

Grid::Grid(size_t width, size_t height, SDL_Rect rect, std::vector<std::unique_ptr<Texture>>&& textures) : _width(width), _height(height), textures(std::move(textures)), start({0, 0}), target({height - 1, width - 1}) {
    auto [x, y, w, h] = rect;

    _tile_size = std::min(h / height, w / width);
    _rect = {x, y, width * _tile_size, height * _tile_size};
    update();
}

Grid* Grid::create(SDL_Renderer* renderer, size_t width, size_t height, SDL_Rect rect) {
    std::vector<std::unique_ptr<Texture>> textures(static_cast<size_t>(Grid::Tex::__COUNT));

    textures[static_cast<size_t>(Grid::Tex::GROUND)] = std::unique_ptr<Texture>(Texture::create(renderer, TexData::ground, TexData::ground_len, {32, 32}, 125));
    textures[static_cast<size_t>(Grid::Tex::OBSTACLE)] = std::unique_ptr<Texture>(Texture::create(renderer, TexData::obstacle, TexData::obstacle_len, {32, 32}, 125));
    textures[static_cast<size_t>(Grid::Tex::START)] = std::unique_ptr<Texture>(Texture::create(renderer, TexData::start, TexData::start_len, {32, 32}, 125));
    textures[static_cast<size_t>(Grid::Tex::TARGET)] = std::unique_ptr<Texture>(Texture::create(renderer, TexData::target, TexData::target_len, {32, 32}, 125));
    textures[static_cast<size_t>(Grid::Tex::VISITED)] = std::unique_ptr<Texture>(Texture::create(renderer, TexData::visited, TexData::visited_len, {32, 32}, 125));
    textures[static_cast<size_t>(Grid::Tex::PATH)] = std::unique_ptr<Texture>(Texture::create(renderer, TexData::path, TexData::path_len, {32, 32}, 125));
    textures[static_cast<size_t>(Grid::Tex::PATH_BORDER)] = std::unique_ptr<Texture>(Texture::create(renderer, TexData::path_border, TexData::path_border_len, {32, 32}, 125));
    textures[static_cast<size_t>(Grid::Tex::CELL_OVERLAY)] = std::unique_ptr<Texture>(Texture::create(renderer, TexData::cell_overlay, TexData::cell_overlay_len, {32, 32}, 125));

    if(std::any_of(textures.begin(), textures.end(), std::logical_not<>{})) {
        return nullptr;
    }

    return new Grid(width, height, rect, std::move(textures));
}

double Grid::heuristic(Cell cell) {
    int dy = std::abs(static_cast<int>(cell.first) - static_cast<int>(target.first));
    int dx = std::abs(static_cast<int>(cell.second) - static_cast<int>(target.second));

    return dx + dy;
}

void Grid::draw_tile(SDL_Renderer* renderer, Grid::Tex texture, Cell cell, double rotation, uint8_t alpha) {
    auto [x, y, _, __] = rect;
    auto [j, i] = cell;

    textures[static_cast<size_t>(texture)]->draw(renderer, {x + i * tile_size, y + j * tile_size, tile_size, tile_size}, rotation, alpha);
}

void Grid::draw(SDL_Renderer* renderer) {
    auto [x, y, _, __] = rect;

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            draw_tile(renderer, Grid::Tex::GROUND, {j, i});
        }
    }

    for(auto const& cell : obstacles) {
        draw_tile(renderer, Grid::Tex::OBSTACLE, cell);
    }

    for(auto const& cell : visited) {
        draw_tile(renderer, Grid::Tex::VISITED, cell);
    }

    for(auto const& cell : path) {
        draw_tile(renderer, Grid::Tex::PATH_BORDER, cell);
    }

    for(size_t i = 1; i < path.size(); i++) {
        auto [py, px] = path[i - 1];
        auto [ty, tx] = path[i];

        if(px != tx) {
            draw_tile(renderer, Grid::Tex::PATH, path[i - 1], 90 + 180 * (px < tx));
            draw_tile(renderer, Grid::Tex::PATH, path[i], 90 + 180 * (px > tx));
        } else { // py != ty
            draw_tile(renderer, Grid::Tex::PATH, path[i - 1], 180 * (py > ty));
            draw_tile(renderer, Grid::Tex::PATH, path[i], 180 * (py < ty));
        }
    }

    draw_tile(renderer, Grid::Tex::START, start);
    draw_tile(renderer, Grid::Tex::TARGET, target);
    draw_tile(renderer, Grid::Tex::CELL_OVERLAY, hover);
    draw_tile(renderer, placeable[selected_tile], hover, 0, 127);
}

void Grid::update() {
    static int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    std::vector<std::vector<Grid::Cell>> parent(height, std::vector<Grid::Cell>(width, {-1, -1}));
    std::vector<std::vector<int>> local_score(height, std::vector<int>(width, INF));
    std::priority_queue<std::pair<double, Grid::Cell>> pq{};

    visited.clear();
    path.clear();

    local_score[start.first][start.second] = 0;
    pq.push({-heuristic(start), start});

    while(!pq.empty()) {
        auto [x, p] = pq.top();
        pq.pop();

        visited.insert(p);

        if(p == target) {
            break;
        }

        for(auto [dy, dx] : dirs) {
            int y = p.first + dy;
            int x = p.second + dx;

            if(y < 0 || y >= height || x < 0 || x >= width || obstacles.find({y, x}) != obstacles.end()) {
                continue;
            }

            if(local_score[p.first][p.second] + 1 < local_score[y][x]) {
                parent[y][x] = p;
                local_score[y][x] = local_score[p.first][p.second] + 1;
                pq.push({-(local_score[y][x] + heuristic({y, x})), {y, x}});
            }
        }
    }

    auto current = target;
    while(current != decltype(parent)::value_type::value_type{-1, -1}) {
        path.push_back(current);
        current = parent[current.first][current.second];
    }
}

void Grid::handle_mouse_motion(SDL_MouseMotionEvent const& e) {
    size_t x = (e.x - rect.x) / tile_size;
    size_t y = (e.y - rect.y) / tile_size;

    hover = {y, x};
}

void Grid::handle_mouse_click(SDL_MouseButtonEvent const& e) {
    size_t x = (e.x - rect.x) / tile_size;
    size_t y = (e.y - rect.y) / tile_size;

    switch(e.button) {
        case SDL_BUTTON_LEFT:
            switch(placeable[selected_tile]) {
                case Grid::Tex::START:
                    set_start(x, y);
                    break;
                case Grid::Tex::TARGET:
                    set_target(x, y);
                    break;
                case Grid::Tex::OBSTACLE:
                    toggle_obstacle(x, y);
                    break;
            }
            break;
        case SDL_BUTTON_RIGHT:
            toggle_obstacle(x, y);
            break;
    }
}

void Grid::handle_mouse_wheel(SDL_MouseWheelEvent const& e) {
    if(e.y > 0) {
        selected_tile = (selected_tile + 1) % placeable.size();
    } else if(e.y < 0) {
        selected_tile = (selected_tile + placeable.size() - 1) % placeable.size();
    }
}

void Grid::set_start(size_t x, size_t y) {
    if(target != decltype(target){y, x}) {
        start = {y, x};

        auto it = obstacles.find(start);
        if(it != obstacles.end()) {
            obstacles.erase(it);
        }

        update();
    }
}

void Grid::set_target(size_t x, size_t y) {
    if(start != decltype(start){y, x}) {
        target = {y, x};

        auto it = obstacles.find(target);
        if(it != obstacles.end()) {
            obstacles.erase(it);
        }

        update();
    }
}

void Grid::toggle_obstacle(size_t x, size_t y) {
    if(start == decltype(start){y, x} || target == decltype(target){y, x}) {
        return;
    }

    auto elem = decltype(obstacles)::value_type{y, x};
    auto it = obstacles.find(elem);

    if(it == obstacles.end()) {
        obstacles.insert(elem);
    } else {
        obstacles.erase(it);
    }

    update();
}