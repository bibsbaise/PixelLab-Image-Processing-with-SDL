#include "Button.hpp"

Button::Button(SDL_Renderer *renderer, TTF_TextEngine *textEngine, TTF_Font *font, const std::string &text,
               SDL_FRect rect, SDL_Color bg, SDL_Color fg, Callback onClick)
    : renderer_(renderer), textEngine_(textEngine), font_(font), label_(text), bg_(bg), fg_(fg),
      onClick_(std::move(onClick)), Component() {
  rect_ = rect;
  rebuildText();
}

Button::~Button() = default;

void Button::handleEvent(const SDL_Event &event) {
  bool now;
  switch (event.type) {
  case SDL_EVENT_MOUSE_MOTION:
    now = contains(event.motion.x, event.motion.y);
    if (now != hovered_) {
      hovered_ = now;
      rebuildText();
    }
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    if (event.button.button == SDL_BUTTON_LEFT && contains(event.button.x, event.button.y)) {
      if (onClick_)
        onClick_();
    }
    break;
  default:
    break;
  }
}

void Button::render(SDL_Renderer *renderer) const {
  SDL_SetRenderDrawColor(renderer, bg_.r, bg_.g, bg_.b, bg_.a);
  SDL_RenderFillRect(renderer, &rect_);

  if (text_) {
    float x = rect_.x + (rect_.w - textW_) / 2;
    float y = rect_.y + (rect_.h - textH_) / 2;
    TTF_DrawRendererText(text_.get(), x, y);
  }
}

void Button::rebuildText() {
  TTF_SetFontStyle(font_, hovered_ ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);

  text_.reset(TTF_CreateText(textEngine_, font_, label_.c_str(), 0));
  if (!text_)
    return;

  TTF_SetTextColor(text_.get(), fg_.r, fg_.g, fg_.b, fg_.a);
  TTF_GetTextSize(text_.get(), &textW_, &textH_);
}
