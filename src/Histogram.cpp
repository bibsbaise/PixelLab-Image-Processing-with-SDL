#include "Histogram.hpp"

Histogram::Histogram(const std::array<uint8_t, 256> &data, int width, int height)
    : Component(), data_(data) {}

Histogram::~Histogram() = default;

void Histogram::render(SDL_Renderer *renderer) const {
  SDL_Log("Histogram render called, but it's not implemented yet");
}
