#pragma once

#include <SDL3/SDL.h>

class Component {
public:
  Component() = default;

  Component(const Component &) = delete;
  Component &operator=(const Component &) = delete;

  Component(Component &&) = delete;
  Component &operator=(Component &&) = delete;

  virtual ~Component() = default;
  virtual void handleEvent(const SDL_Event &event) = 0;
  virtual void render(SDL_Renderer *renderer) const = 0;

  bool contains(float x, float y) const {
    return x >= rect_.x && x <= rect_.x + rect_.w && y >= rect_.y && y <= rect_.y + rect_.h;
  }

protected:
  SDL_FRect rect_{};
};
