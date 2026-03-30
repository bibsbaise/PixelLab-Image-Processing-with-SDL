#include "Label.hpp"

Label::Label(SDL_Renderer *renderer, TTF_TextEngine *textEngine, TTF_Font *font, const std::string &text,
             SDL_FRect rect, SDL_Color textColor)
    : Component(rect), renderer_(renderer), textEngine_(textEngine), font_(font), label_(text),
      textColor_(textColor) {}

Label::~Label() = default;

void Label::setText(const std::string &text) { label_ = text; }

void Label::handleEvent(const SDL_Event &event) {}

void Label::render() const {
  const auto &text = rebuildText();
  if (text) {
    TTF_DrawRendererText(text.get(), rect_.x, rect_.y);
  }
}

const TextPtr Label::rebuildText() const {
  auto text = TTF_CreateText(textEngine_, font_, label_.c_str(), 0);
  if (!text) {
    SDL_Log("Failed to create text for label: %s", SDL_GetError());
    return nullptr;
  }

  TTF_SetTextColor(text, textColor_.r, textColor_.g, textColor_.b, textColor_.a);
  return TextPtr(text);
}
