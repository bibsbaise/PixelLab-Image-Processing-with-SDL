#pragma once

#include "SDLDeleter.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <array>

SurfacePtr convertToGrayscale(const SurfacePtr &originalSurface);
TexturePtr surfaceToTexture(const RendererPtr &renderer, const SurfacePtr &surface);
SurfacePtr equalizeHistogram(const SurfacePtr &graySurface);

std::array<int, 256> calculateHistogram(SDL_Surface *surface);
std::array<int, 256> calculateCDF(const std::array<int, 256> &hist);

float calcularMediaHistograma(const std::array<int, 256> &vetorIntensidade);
int classificarBrilho(float media);
float calcularDesvioPadrao(const std::array<int, 256> &vetorIntensidade, float media);
int classificarContraste(float desvioPadrao);

std::string classificarBrilhoTexto(float media);
std::string classificarContrasteTexto(float desvio);