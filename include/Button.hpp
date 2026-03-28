#pragma once

#include "Component.hpp"
#include "SDLDeleter.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <string>

class Button : public Component {
public:
  ~Button();

  using Callback = std::function<void()>;

  Button(SDL_Renderer *renderer, TTF_TextEngine *textEngine, TTF_Font *font, const std::string &text,
         SDL_FRect rect, SDL_Color bg, SDL_Color fg, Callback onClick);

  void handleEvent(const SDL_Event &event) override;
  void render(SDL_Renderer *renderer) const override;

private:
  void rebuildText();

  SDL_Renderer *renderer_;
  TTF_TextEngine *textEngine_;
  TTF_Font *font_;

  std::string label_;
  SDL_Color bg_;
  SDL_Color fg_;
  Callback onClick_;
  bool hovered_ = false;

  TextPtr text_;
  int textW_ = 0, textH_ = 0;
};
