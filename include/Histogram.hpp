#pragma once

#include "Component.hpp"
#include <vector>

class Histogram : public Component {
public:
  Histogram(const std::array<uint8_t, 256> &data, int width, int height);
  ~Histogram();

  void render(SDL_Renderer *renderer) const override;

private:
  const std::array<uint8_t, 256> data_;
};
