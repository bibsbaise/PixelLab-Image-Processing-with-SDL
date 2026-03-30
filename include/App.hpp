#pragma once

#include "Button.hpp"
#include "Histogram.hpp"
#include "SDLDeleter.hpp"
#include <memory>
#include <string>

// dimensão das janelas
constexpr int kMaxWindowSize = 720;
constexpr int kToolsWindowWidth = 480;
constexpr int kToolsWindowHeight = 360;

// cor de fundo das janelas
constexpr SDL_Color kWindowBgColor{240, 242, 245, 255}; // #f0f2f5

// config do butão
constexpr SDL_Color kButtonTextColor{255, 255, 255, 255};   // #FFFFFF
constexpr SDL_Color kButtonBgColor{0, 123, 255, 255};       // #007bff
constexpr SDL_Color kButtonHoverBgColor{51, 149, 255, 255}; // #3395FF
constexpr SDL_Color kButtonPressBgColor{0, 86, 179, 255};   // #0056B3

// config do histograma
constexpr SDL_Color kHistogramBarColor{102, 102, 102, 255}; // #666666
constexpr SDL_Color kHistogramOutlineColor{0, 0, 0, 255};   // #000000

// textos
constexpr char kWindowDefaultTitle[] = "PixelLab - Grayscale";
constexpr char kWindowEqualizedTitle[] = "PixelLab - Equalized";
constexpr char kToolsWindowTitle[] = "PixelLab - Tools";
constexpr char kEqualizeText[] = "Equalizar";
constexpr char kEqualizeOffText[] = "Ver original";

class App {
public:
  App();
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  App(App &&) = delete;
  App &operator=(App &&) = delete;

  bool init(const std::string &imagePath);
  void run();

private:
  bool loadImage(const std::string &imagePath);
  bool createTextureFromCurrent();
  void toggleEqualization();
  void handleEvents(bool &running);
  void handleKeyDown(const SDL_KeyboardEvent &e, bool &running);
  void render();
  void positionWindows();

private:
  // monte de ponteiros inteligentes para gerenciar recursos do SDL
  WindowPtr mainWindow_;
  WindowPtr toolsWindow_;
  RendererPtr mainRenderer_;
  RendererPtr toolsRenderer_;
  TextEnginePtr textEngine_;
  TexturePtr textTexture_;
  FontPtr font_;
  SurfacePtr originalGraySurface_;
  SurfacePtr equalizedSurface_;
  TexturePtr imageTexture_;

  // equalização
  bool isEqualized_ = false;
  std::array<int, 256> histogramGrayscaleData_{};
  std::array<int, 256> histogramEqualizedData_{};

  // componentes da interface
  std::unique_ptr<Button> btnToggleEqualization_;
  std::unique_ptr<Histogram> histogram_;
};
