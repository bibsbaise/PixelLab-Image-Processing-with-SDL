#include "equalization.h"
#include <SDL3/SDL.h>
#include <iostream>

SDL_Surface* equalizeHistogram(SDL_Surface* graySurface) {
    if (!graySurface) {
        SDL_Log("equalizeHistogram: surface nula");
        return nullptr;
    }

    SDL_Surface* src = SDL_ConvertSurface(graySurface, SDL_PIXELFORMAT_ARGB8888);
    if (!src) {
        SDL_Log("Erro ao converter surface para equalizacao: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Surface* result = SDL_CreateSurface(src->w, src->h, SDL_PIXELFORMAT_ARGB8888);
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

    Uint32* srcPixels = static_cast<Uint32*>(src->pixels);
    Uint32* dstPixels = static_cast<Uint32*>(result->pixels);
    const int totalPixels = src->w * src->h;

    int hist[256] = {0};
    int cdf[256] = {0};

    for (int i = 0; i < totalPixels; ++i) {
        Uint32 pixel = srcPixels[i];
        Uint8 gray = pixel & 0xFF;
        hist[gray]++;
    }

    cdf[0] = hist[0];
    for (int i = 1; i < 256; ++i) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    int cdfMin = 0;
    for (int i = 0; i < 256; ++i) {
        if (cdf[i] != 0) {
            cdfMin = cdf[i];
            break;
        }
    }

    int denominator = totalPixels - cdfMin;
    if (denominator <= 0) {
        SDL_Log("equalizeHistogram: imagem sem variacao suficiente para equalizacao");
        SDL_UnlockSurface(result);
        SDL_UnlockSurface(src);
        SDL_DestroySurface(src);
        return result;
    }

    for (int i = 0; i < totalPixels; ++i) {
        Uint32 pixel = srcPixels[i];
        Uint8 a = (pixel >> 24) & 0xFF;
        Uint8 gray = pixel & 0xFF;

        int mapped = (cdf[gray] - cdfMin) * 255 / denominator;
        if (mapped < 0) mapped = 0;
        if (mapped > 255) mapped = 255;

        Uint32 newPixel =
            (static_cast<Uint32>(a) << 24) |
            (static_cast<Uint32>(mapped) << 16) |
            (static_cast<Uint32>(mapped) << 8) |
            (static_cast<Uint32>(mapped));

        dstPixels[i] = newPixel;
    }

    SDL_UnlockSurface(result);
    SDL_UnlockSurface(src);
    SDL_DestroySurface(src);

    return result;
}