#include "utils.hpp"

bool encloses(SDL_Rect rect, SDL_Point point) {
    return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}