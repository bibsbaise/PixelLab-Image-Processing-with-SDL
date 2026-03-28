#include "App.hpp"

App::App() = default;

App::~App() {
  font_.reset();
  textTexture_.reset();
  renderer_.reset();
  window_.reset();
  btnClickMe_.reset();

  TTF_Quit();
  SDL_Quit();
}

bool App::init() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return false;
  }

  if (!TTF_Init()) {
    SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
    SDL_Quit();
    return false;
  }

  SDL_Window *rawWindow = nullptr;
  SDL_Renderer *rawRenderer = nullptr;

  if (!SDL_CreateWindowAndRenderer("PixelLab", 800, 600, 0, &rawWindow, &rawRenderer)) {
    SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  window_.reset(rawWindow);
  renderer_.reset(rawRenderer);

  const char *fontPath = SDL_getenv("APP_FONT_PATH");
  if (!fontPath || !*fontPath) {
#if defined(__APPLE__)
    fontPath = "/System/Library/Fonts/Monaco.ttf";
#elif defined(_WIN32)
    fontPath = "C:\\Windows\\Fonts\\arial.ttf";
#elif defined(__linux__)
    fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#else
    SDL_Log("No default font path for this platform. Please set APP_FONT_PATH environment variable.");
    renderer_.reset();
    window_.reset();
    TTF_Quit();
    SDL_Quit();
    return false;
#endif
  }

  font_.reset(TTF_OpenFont(fontPath, 24));
  if (!font_) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
    renderer_.reset();
    window_.reset();
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  textEngine_.reset(TTF_CreateRendererTextEngine(rawRenderer));
  if (!textEngine_) {
    SDL_Log("Failed to create text engine: %s", SDL_GetError());
    font_.reset();
    renderer_.reset();
    window_.reset();
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  btnClickMe_.reset(new Button(renderer_.get(), textEngine_.get(), font_.get(), "Clica eu",
                               SDL_FRect{300, 200, 200, 50}, SDL_Color{100, 149, 237, 255} /* #6495ed */,
                               SDL_Color{255, 255, 255, 255} /* #fff */,
                               []() { SDL_Log("Button clicked!"); }));

  return true;
}

void App::run() {
  bool running = true;

  while (running) {
    handleEvents(running);
    render();
  }
}

void App::handleEvents(bool &running) {
  SDL_Event e{};

  // trocar a desgraça do SDL_PollEvent pelo SDL_WaitEvent
  // fez o SDL parar de esquentar o mac com o app idle
  // SDL_PollEvent -> jogos ou app com interacao pesada
  // SDL_WaitEvent -> ui q pode esperar por input do usuario
  if (SDL_WaitEvent(&e)) {
    switch (e.type) {
    case SDL_EVENT_QUIT:
      running = false;
      break;
    case SDL_EVENT_KEY_DOWN:
      handleKeyDown(e.key, running);
      break;
    default:
      break;
    }
    btnClickMe_->handleEvent(e);
  }
}

void App::handleKeyDown(const SDL_KeyboardEvent &e, bool &running) {
  switch (e.key) {
  case SDLK_S:
    SDL_Log("Key 'S' pressed but save logic is not implemented yet");
    // TODO: implementar logica de salvar imagem processada
    break;
  case SDLK_ESCAPE:
    SDL_Log("Escape key pressed, exiting...");
    running = false;
    break;
  default:
    break;
  }
}

void App::render() {
  const SDL_Color bg = {204, 204, 204, 255}; // #ccc
  SDL_SetRenderDrawColor(renderer_.get(), bg.r, bg.g, bg.b, bg.a);
  SDL_RenderClear(renderer_.get());

  btnClickMe_->render(renderer_.get());

  SDL_RenderPresent(renderer_.get());
};
