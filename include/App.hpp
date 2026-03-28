#pragma once

#include "Button.hpp"
#include "SDLDeleter.hpp"
#include <memory>

class App {
public:
  App();
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  App(App &&) = delete;
  App &operator=(App &&) = delete;

  bool init();
  void run();

private:
  void handleEvents(bool &running);
  void handleKeyDown(const SDL_KeyboardEvent &e, bool &running);
  void render();

private:
  WindowPtr window_;
  RendererPtr renderer_;
  TextEnginePtr textEngine_;
  TexturePtr textTexture_;
  FontPtr font_;

  std::unique_ptr<Button> btnClickMe_;
};
