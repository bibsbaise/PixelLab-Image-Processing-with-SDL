#include "Button.hpp"

Button::Button(SDL_Renderer *renderer, TTF_TextEngine *textEngine, TTF_Font *font, const std::string &text,
               SDL_FRect rect, SDL_Color textColor, SDL_Color bg, SDL_Color hoverBg, SDL_Color pressBg,
               Callback onClick)
    : renderer_(renderer), textEngine_(textEngine), font_(font), label_(text), textColor_(textColor), bg_(bg),
      hoverBg_(hoverBg), pressBg_(pressBg), onClick_(std::move(onClick)), Component() {
  rect_ = rect;
}

Button::~Button() = default;

void Button::handleEvent(const SDL_Event &event) {
  switch (event.type) {
  case SDL_EVENT_MOUSE_MOTION:
    hovered_ = contains(event.motion.x, event.motion.y);
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    if (event.button.button == SDL_BUTTON_LEFT && contains(event.button.x, event.button.y)) {
      pressed_ = true;
      if (onClick_)
        onClick_();
    }
    break;
  case SDL_EVENT_MOUSE_BUTTON_UP:
    if (event.button.button == SDL_BUTTON_LEFT) {
      pressed_ = false;
    }
    break;
  default:
    break;
  }
}

void Button::setText(const std::string &text) { label_ = text; }

void Button::render() const {
  SDL_Color currentBg = pressed_ ? pressBg_ : hovered_ ? hoverBg_ : bg_;
  SDL_SetRenderDrawColor(renderer_, currentBg.r, currentBg.g, currentBg.b, currentBg.a);
  SDL_RenderFillRect(renderer_, &rect_);

  const auto &text = rebuildText();

  if (text) {
    int textW, textH;
    TTF_GetTextSize(text.get(), &textW, &textH);
    float x = rect_.x + (rect_.w - textW) / 2;
    float y = rect_.y + (rect_.h - textH) / 2;
    TTF_DrawRendererText(text.get(), x, y);
  }
}

const TextPtr Button::rebuildText() const {
  auto text = TTF_CreateText(textEngine_, font_, label_.c_str(), 0);
  if (!text) {
    SDL_Log("Failed to create text for button: %s", SDL_GetError());
    return nullptr;
  }

  TTF_SetTextColor(text, textColor_.r, textColor_.g, textColor_.b, textColor_.a);
  return TextPtr(text);
}
