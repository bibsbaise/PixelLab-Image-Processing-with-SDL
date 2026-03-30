#include "App.hpp"
#include "ImageProcessing.hpp"

#include <SDL3_image/SDL_image.h>

App::App() = default;

App::~App() {
  btnToggleEqualization_.reset();
  imageTexture_.reset();
  originalGraySurface_.reset();
  equalizedSurface_.reset();
  font_.reset();
  textTexture_.reset();
  mainRenderer_.reset();
  mainWindow_.reset();

  toolsWindow_.reset();

  TTF_Quit();
  SDL_Quit();
}

bool App::init(const std::string &imagePath) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return false;
  }

  if (!TTF_Init()) {
    SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
    SDL_Quit();
    return false;
  }

  loadImage(imagePath);

  SDL_Window *rawMainWindow = nullptr;
  SDL_Renderer *rawMainRenderer = nullptr;

  int mainWindowWidth = originalGraySurface_->w, mainWindowHeight = originalGraySurface_->h;
  if (mainWindowWidth > kMaxWindowSize || mainWindowHeight > kMaxWindowSize) {
    float aspectRatio = static_cast<float>(kMaxWindowSize) / std::max(mainWindowWidth, mainWindowHeight);
    mainWindowWidth = static_cast<int>(mainWindowWidth * aspectRatio);
    mainWindowHeight = static_cast<int>(mainWindowHeight * aspectRatio);
    SDL_Log("Redimensionando imagem/janela para caber na tela: %dx%d", mainWindowWidth, mainWindowHeight);
  }

  if (!SDL_CreateWindowAndRenderer(kWindowDefaultTitle, mainWindowWidth, mainWindowHeight,
                                   SDL_WINDOW_INPUT_FOCUS, &rawMainWindow, &rawMainRenderer)) {
    SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  SDL_Window *rawToolsWindow;
  SDL_Renderer *rawToolsRenderer;

  if (!SDL_CreateWindowAndRenderer(kToolsWindowTitle, kToolsWindowWidth, kToolsWindowHeight,
                                   SDL_WINDOW_UTILITY, &rawToolsWindow, &rawToolsRenderer)) {
    SDL_Log("Failed to create tools window and renderer: %s", SDL_GetError());
    SDL_DestroyRenderer(rawMainRenderer);
    SDL_DestroyWindow(rawMainWindow);
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  mainWindow_.reset(rawMainWindow);
  mainRenderer_.reset(rawMainRenderer);
  toolsWindow_.reset(rawToolsWindow);
  toolsRenderer_.reset(rawToolsRenderer);

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
    mainRenderer_.reset();
    mainWindow_.reset();
    toolsWindow_.reset();
    toolsRenderer_.reset();
    TTF_Quit();
    SDL_Quit();
    return false;
#endif
  }

  font_.reset(TTF_OpenFont(fontPath, 24));
  if (!font_) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
    mainRenderer_.reset();
    mainWindow_.reset();
    toolsWindow_.reset();
    toolsRenderer_.reset();
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  textEngine_.reset(TTF_CreateRendererTextEngine(rawToolsRenderer));
  if (!textEngine_) {
    SDL_Log("Failed to create text engine: %s", SDL_GetError());
    font_.reset();
    mainRenderer_.reset();
    mainWindow_.reset();
    toolsWindow_.reset();
    toolsRenderer_.reset();
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  createTextureFromCurrent();
  histogram_ =
      std::make_unique<Histogram>(rawToolsRenderer, histogramGrayscaleData_, SDL_FRect{20, 20, 440, 240},
                                  kHistogramBarColor, kHistogramOutlineColor);
  btnToggleEqualization_ =
      std::make_unique<Button>(rawToolsRenderer, textEngine_.get(), font_.get(), kEqualizeText,
                               SDL_FRect{240, 280, 220, 50}, kButtonTextColor, kButtonBgColor,
                               kButtonHoverBgColor, kButtonPressBgColor, [this]() { toggleEqualization(); });

  positionWindows();

  return true;
}

bool App::loadImage(const std::string &imagePath) {
  SurfacePtr loadedSurface(IMG_Load(imagePath.c_str()));

  if (!loadedSurface) {
    SDL_Log("Failed to load image '%s': %s", imagePath.c_str(), SDL_GetError());
    return false;
  }

  originalGraySurface_ = convertToGrayscale(loadedSurface);
  if (!originalGraySurface_) {
    SDL_Log("Failed to convert image to grayscale");
    return false;
  }

  histogramGrayscaleData_ = calculateHistogram(originalGraySurface_.get());

  return true;
}

bool App::createTextureFromCurrent() {
  SDL_Surface *currentSurface = isEqualized_ ? equalizedSurface_.get() : originalGraySurface_.get();

  imageTexture_.reset(SDL_CreateTextureFromSurface(mainRenderer_.get(), currentSurface));
  if (!imageTexture_) {
    SDL_Log("Failed to create image texture: %s", SDL_GetError());
    return false;
  }

  return true;
}

void App::toggleEqualization() {
  if (!originalGraySurface_) {
    return;
  }

  if (!equalizedSurface_) {
    equalizedSurface_ = equalizeHistogram(originalGraySurface_);
    histogramEqualizedData_ = calculateHistogram(equalizedSurface_.get());
  }

  if (!isEqualized_) {
    isEqualized_ = true;
    SDL_SetWindowTitle(mainWindow_.get(), kWindowEqualizedTitle);
    histogram_->setData(histogramEqualizedData_);
    btnToggleEqualization_->setText(kEqualizeOffText);
  } else {
    isEqualized_ = false;
    SDL_SetWindowTitle(mainWindow_.get(), kWindowDefaultTitle);
    histogram_->setData(histogramGrayscaleData_);
    btnToggleEqualization_->setText(kEqualizeText);
  }

  createTextureFromCurrent();
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

    btnToggleEqualization_->handleEvent(e);
  }
}

void App::handleKeyDown(const SDL_KeyboardEvent &e, bool &running) {
  switch (e.key) {
  case SDLK_E:
    toggleEqualization();
    break;
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
  // limpa com a cor de fundo antes de renderizar
  SDL_SetRenderDrawColor(mainRenderer_.get(), kWindowBgColor.r, kWindowBgColor.g, kWindowBgColor.b,
                         kWindowBgColor.a);
  SDL_RenderClear(mainRenderer_.get());
  SDL_SetRenderDrawColor(toolsRenderer_.get(), kWindowBgColor.r, kWindowBgColor.g, kWindowBgColor.b,
                         kWindowBgColor.a);
  SDL_RenderClear(toolsRenderer_.get());

  // renderiza a imagem
  if (imageTexture_) {
    SDL_RenderTexture(mainRenderer_.get(), imageTexture_.get(), nullptr, nullptr);
  }

  // renderiza os componentes
  btnToggleEqualization_->render();
  histogram_->render();

  // apresenta tudo o que foi renderizado
  SDL_RenderPresent(mainRenderer_.get());
  SDL_RenderPresent(toolsRenderer_.get());
};

void App::positionWindows() {
  if (!mainWindow_ || !toolsWindow_) {
    SDL_Log("Cannot position windows, one or both of them are null");
    return;
  }
  SDL_SetWindowPosition(mainWindow_.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  int x, y, width, height;
  SDL_GetWindowSize(mainWindow_.get(), &width, &height);
  SDL_GetWindowPosition(mainWindow_.get(), &x, &y);

  int toolsX = x + width, toolsY = y + (height - kToolsWindowHeight) / 2;
  SDL_SetWindowPosition(toolsWindow_.get(), toolsX, toolsY);

  // traz pra frente as janelas
  SDL_RaiseWindow(mainWindow_.get());
  SDL_RaiseWindow(toolsWindow_.get());
}
