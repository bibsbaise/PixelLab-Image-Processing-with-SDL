#include "Histogram.hpp"
#include <algorithm>

Histogram::Histogram(SDL_Renderer *renderer, const std::array<int, 256> &data, SDL_FRect rect,
                     SDL_Color barColor, SDL_Color outlineColor)
    : Component(rect), renderer_(renderer), data_(data), barColor_(barColor), outlineColor_(outlineColor) {}

Histogram::~Histogram() = default;

void Histogram::handleEvent(const SDL_Event &event) {}

void Histogram::setData(const std::array<int, 256> &data) { data_ = data; }

void Histogram::render() const {
  int maxCount = *std::max_element(data_.begin(), data_.end());
  int size = data_.size();

  if (maxCount > 0) {
    SDL_SetRenderDrawColor(renderer_, barColor_.r, barColor_.g, barColor_.b, barColor_.a);

    // renderiza cada barrinha do histograma
    float barWidth = rect_.w / size;
    for (int i = 0; i < size; ++i) {
      if (data_[i] > 0) {
        float barHeight = (static_cast<float>(data_[i]) / maxCount) * rect_.h;
        float barX = rect_.x + (static_cast<float>(i) / size) * rect_.w;
        SDL_FRect barRect = {barX, rect_.y + rect_.h - barHeight, barWidth, barHeight};
        SDL_RenderFillRect(renderer_, &barRect);
      }
    }
  }

  // desenha uma borda ao redor do histograma
  SDL_SetRenderDrawColor(renderer_, outlineColor_.r, outlineColor_.g, outlineColor_.b, outlineColor_.a);
  SDL_RenderRect(renderer_, &rect_);
}
