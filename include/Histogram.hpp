#pragma once

#include "Component.hpp"
#include <array>

class Histogram : public Component {
public:
  Histogram(SDL_Renderer *renderer, const std::array<int, 256> &data, SDL_FRect rect, SDL_Color barColor,
            SDL_Color outlineColor);
  ~Histogram();

  void handleEvent(const SDL_Event &event) override;
  void setData(const std::array<int, 256> &data);
  void render() const override;

private:
  SDL_Renderer *renderer_;

  std::array<int, 256> data_;

  SDL_Color barColor_;
  SDL_Color outlineColor_;
};
