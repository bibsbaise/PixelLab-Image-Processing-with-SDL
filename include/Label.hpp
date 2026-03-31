#pragma once

#include "Component.hpp"
#include "SDLDeleter.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

class Label : public Component {
public:
  Label(SDL_Renderer *renderer, TTF_TextEngine *textEngine, TTF_Font *font, const std::string &text,
        SDL_FRect rect, SDL_Color textColor);
  ~Label() override;

  void setText(const std::string &text);
  void handleEvent(const SDL_Event &event) override;
  void render() const override;

private:
  const TextPtr rebuildText() const;

  SDL_Renderer *renderer_;
  TTF_TextEngine *textEngine_;
  TTF_Font *font_;

  std::string label_;
  SDL_Color textColor_;
};
