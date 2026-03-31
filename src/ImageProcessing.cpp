#include "ImageProcessing.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

SurfacePtr convertToGrayscale(const SurfacePtr &originalSurface) {
  if (!originalSurface) {
    SDL_Log("Surface nula");
    return nullptr;
  }

  SDL_Surface *converted = SDL_ConvertSurface(originalSurface.get(), SDL_PIXELFORMAT_ARGB8888);
  if (!converted) {
    SDL_Log("Erro ao converter formato da imagem: %s", SDL_GetError());
    return nullptr;
  }

  SDL_Surface *graySurface = SDL_CreateSurface(converted->w, converted->h, SDL_PIXELFORMAT_ARGB8888);
  if (!graySurface) {
    SDL_Log("Erro ao criar surface grayscale: %s", SDL_GetError());
    SDL_DestroySurface(converted);
    return nullptr;
  }

  if (!SDL_LockSurface(converted)) {
    SDL_Log("Erro ao bloquear surface original: %s", SDL_GetError());
    SDL_DestroySurface(converted);
    SDL_DestroySurface(graySurface);
    return nullptr;
  }

  if (!SDL_LockSurface(graySurface)) {
    SDL_Log("Erro ao bloquear surface grayscale: %s", SDL_GetError());
    SDL_UnlockSurface(converted);
    SDL_DestroySurface(converted);
    SDL_DestroySurface(graySurface);
    return nullptr;
  }

  uint32_t *srcPixels = static_cast<uint32_t *>(converted->pixels);
  uint32_t *dstPixels = static_cast<uint32_t *>(graySurface->pixels);
  const int totalPixels = converted->w * converted->h;

  for (int i = 0; i < totalPixels; ++i) {
    uint32_t pixel = srcPixels[i];

    uint8_t a = (pixel >> 24) & 0xFF;
    uint8_t r = (pixel >> 16) & 0xFF;
    uint8_t g = (pixel >> 8) & 0xFF;
    uint8_t b = pixel & 0xFF;

    uint8_t gray = static_cast<uint8_t>(0.2125f * r + 0.7154f * g + 0.0721f * b);

    uint32_t grayPixel = (static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(gray) << 16) |
                         (static_cast<uint32_t>(gray) << 8) | (static_cast<uint32_t>(gray));

    dstPixels[i] = grayPixel;
  }

  SDL_UnlockSurface(graySurface);
  SDL_UnlockSurface(converted);
  SDL_DestroySurface(converted);

  return SurfacePtr(graySurface);
}

TexturePtr surfaceToTexture(const RendererPtr &renderer, const SurfacePtr &surface) {
  if (!renderer || !surface) {
    SDL_Log("surfaceToTexture: renderer ou surface nulos");
    return nullptr;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer.get(), surface.get());
  if (!texture) {
    SDL_Log("Erro ao criar textura: %s", SDL_GetError());
    return nullptr;
  }

  return TexturePtr(texture);
}

SurfacePtr equalizeHistogram(const SurfacePtr &graySurface) {
  if (!graySurface) {
    SDL_Log("Surface nula");
    return nullptr;
  }

  SDL_Surface *src = SDL_ConvertSurface(graySurface.get(), SDL_PIXELFORMAT_ARGB8888);
  if (!src) {
    SDL_Log("Erro ao converter surface para equalização: %s", SDL_GetError());
    return nullptr;
  }

  SDL_Surface *result = SDL_CreateSurface(src->w, src->h, SDL_PIXELFORMAT_ARGB8888);
  if (!result) {
    SDL_Log("Erro ao criar surface de resultado: %s", SDL_GetError());
    SDL_DestroySurface(src);
    return nullptr;
  }

  if (!SDL_LockSurface(src)) {
    SDL_Log("Erro ao bloquear surface de origem: %s", SDL_GetError());
    SDL_DestroySurface(src);
    SDL_DestroySurface(result);
    return nullptr;
  }

  if (!SDL_LockSurface(result)) {
    SDL_Log("Erro ao bloquear surface de resultado: %s", SDL_GetError());
    SDL_UnlockSurface(src);
    SDL_DestroySurface(src);
    SDL_DestroySurface(result);
    return nullptr;
  }

  uint32_t *srcPixels = static_cast<uint32_t *>(src->pixels);
  uint32_t *dstPixels = static_cast<uint32_t *>(result->pixels);
  const int totalPixels = src->w * src->h;

  const auto &hist = calculateHistogram(src);
  const auto &cdf = calculateCDF(hist);

  int cdfMin = 0;
  for (int i = 0; i < 256; ++i) {
    if (cdf[i] != 0) {
      cdfMin = cdf[i];
      break;
    }
  }

  int denominator = totalPixels - cdfMin;
  if (denominator <= 0) {
    SDL_Log("Imagem sem variação suficiente para equalização");
    SDL_UnlockSurface(result);
    SDL_UnlockSurface(src);
    SDL_DestroySurface(src);
    return SurfacePtr(result);
  }

  for (int i = 0; i < totalPixels; ++i) {
    uint32_t pixel = srcPixels[i];
    uint8_t a = (pixel >> 24) & 0xFF;
    uint8_t gray = pixel & 0xFF;

    int mappedF = (cdf[gray] - cdfMin) * 255.f / denominator;
    int mapped = static_cast<int>(std::round(mappedF));
    if (mapped < 0)
      mapped = 0;
    if (mapped > 255)
      mapped = 255;

    uint32_t newPixel = (static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(mapped) << 16) |
                        (static_cast<uint32_t>(mapped) << 8) | (static_cast<uint32_t>(mapped));

    dstPixels[i] = newPixel;
  }

  SDL_UnlockSurface(result);
  SDL_UnlockSurface(src);
  SDL_DestroySurface(src);

  return SurfacePtr(result);
}

std::array<int, 256> calculateHistogram(SDL_Surface *surface) {
  int totalPixels = surface->w * surface->h;
  std::array<int, 256> hist;
  hist.fill(0);
  auto pixels = static_cast<uint32_t *>(surface->pixels);
  for (int i = 0; i < totalPixels; ++i) {
    uint8_t gray = pixels[i] & 0xFF;
    hist[gray]++;
  }

  return hist;
}

std::array<int, 256> calculateCDF(const std::array<int, 256> &hist) {
  std::array<int, 256> cdf;
  cdf[0] = hist[0];
  for (int i = 1; i < 256; ++i) {
    cdf[i] = cdf[i - 1] + hist[i];
  }
  return cdf;
}

float calcularMediaHistograma(const std::array<int, 256> &vetorIntensidade) {
  int soma = 0;
  int totalPixels = 0;

  for (int i = 0; i < 256; ++i) {
    soma += vetorIntensidade[i] * i; // multiplicamos a quantidade de pixels pela intensidade correspondente
    totalPixels += vetorIntensidade[i]; // somamos a quantidade total de pixels
  }

  if (totalPixels == 0)
    return 0.0f; // evitar divisão por zero

  return static_cast<float>(soma) / totalPixels; // calculamos a média dividindo a soma pelo total de pixels
}

int classificarBrilho(float media) {
  if (media < 85.0f)
    return 0; // imagem escura
  else if (media < 170.0f)
    return 1; // imagem média
  else
    return 2; // imagem clara
}

float calcularDesvioPadrao(const std::array<int, 256> &vetorIntensidade, float media) {
  int totalPixels = 0;
  float somaQuadrados = 0.0f;

  for (int i = 0; i < 256; ++i) {
    totalPixels += vetorIntensidade[i]; // somamos a quantidade total de pixels
    somaQuadrados += vetorIntensidade[i] * (i - media) *
                     (i - media); // somamos o quadrado da diferença entre a intensidade e a média,
                                  // multiplicado pela quantidade de pixels daquela intensidade
  }

  if (totalPixels == 0)
    return 0.0f; // evitar divisão por zero

  return sqrtf(somaQuadrados / totalPixels); // calculamos o desvio padrão dividindo a soma dos quadrados pelo
                                             // total de pixels e tirando a raiz quadrada
}

int classificarContraste(float desvioPadrao) {
  if (desvioPadrao < 50.0f)
    return 0; // baixo contraste
  else if (desvioPadrao < 100.0f)
    return 1; // contraste médio
  else
    return 2; // alto contraste
}
