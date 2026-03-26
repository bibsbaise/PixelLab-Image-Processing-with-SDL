#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <cstdint>
#include "equalization.h"

/*
Para compilar no PowerShell:
g++ src/main.cpp src/equalization.cpp -o pixel_lab.exe -I"C:/Users/bibiz/Downloads/compvisual/SDL3/include" -I"C:/Users/bibiz/Downloads/compvisual/SDL3_image/include" -Isrc -L"C:/Users/bibiz/Downloads/compvisual/SDL3/lib" -L"C:/Users/bibiz/Downloads/compvisual/SDL3_image/lib" -lSDL3 -lSDL3_image -mconsole

Para rodar:
.\pixel_lab.exe assets/teste.jpg
*/

static SDL_Surface* convertToGrayscale(SDL_Surface* originalSurface) {
    if (!originalSurface) {
        SDL_Log("convertToGrayscale: surface nula");
        return nullptr;
    }

    SDL_Surface* converted = SDL_ConvertSurface(originalSurface, SDL_PIXELFORMAT_ARGB8888);
    if (!converted) {
        SDL_Log("Erro ao converter formato da imagem: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Surface* graySurface = SDL_CreateSurface(converted->w, converted->h, SDL_PIXELFORMAT_ARGB8888);
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

    Uint32* srcPixels = static_cast<Uint32*>(converted->pixels);
    Uint32* dstPixels = static_cast<Uint32*>(graySurface->pixels);
    const int totalPixels = converted->w * converted->h;

    for (int i = 0; i < totalPixels; ++i) {
        Uint32 pixel = srcPixels[i];

        Uint8 a = (pixel >> 24) & 0xFF;
        Uint8 r = (pixel >> 16) & 0xFF;
        Uint8 g = (pixel >> 8) & 0xFF;
        Uint8 b = pixel & 0xFF;

        Uint8 gray = static_cast<Uint8>(0.2125f * r + 0.7154f * g + 0.0721f * b);

        Uint32 grayPixel =
            (static_cast<Uint32>(a) << 24) |
            (static_cast<Uint32>(gray) << 16) |
            (static_cast<Uint32>(gray) << 8) |
            (static_cast<Uint32>(gray));

        dstPixels[i] = grayPixel;
    }

    SDL_UnlockSurface(graySurface);
    SDL_UnlockSurface(converted);
    SDL_DestroySurface(converted);

    return graySurface;
}

static SDL_Texture* surfaceToTexture(SDL_Renderer* renderer, SDL_Surface* surface) {
    if (!renderer || !surface) {
        SDL_Log("surfaceToTexture: renderer ou surface nulos");
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Erro ao criar textura: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}

static void replaceTexture(SDL_Renderer* renderer, SDL_Texture*& texture, SDL_Surface* surface, SDL_FRect& rect) {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    texture = surfaceToTexture(renderer, surface);
    if (texture) {
        SDL_GetTextureSize(texture, &rect.w, &rect.h);
        rect.x = 0.0f;
        rect.y = 0.0f;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: programa caminho_da_imagem\n";
        return 1;
    }

    const char* imagePath = argv[1];

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Erro SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Surface* loadedSurface = IMG_Load(imagePath);
    if (!loadedSurface) {
        SDL_Log("Erro ao carregar imagem '%s': %s", imagePath, SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface* originalGraySurface = convertToGrayscale(loadedSurface);
    SDL_DestroySurface(loadedSurface);

    if (!originalGraySurface) {
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "PixelLab - Grayscale",
        originalGraySurface->w,
        originalGraySurface->h,
        0
    );

    if (!window) {
        SDL_Log("Erro ao criar janela: %s", SDL_GetError());
        SDL_DestroySurface(originalGraySurface);
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Erro ao criar renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroySurface(originalGraySurface);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* currentTexture = surfaceToTexture(renderer, originalGraySurface);
    if (!currentTexture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroySurface(originalGraySurface);
        SDL_Quit();
        return 1;
    }

    SDL_FRect rect;
    rect.x = 0.0f;
    rect.y = 0.0f;
    SDL_GetTextureSize(currentTexture, &rect.w, &rect.h);

    std::cout << "Imagem aberta com sucesso!\n";

    bool isEqualized = false;

    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_E) {
                        if (!isEqualized) {
                            SDL_Surface* equalizedSurface = equalizeHistogram(originalGraySurface);
                            if (equalizedSurface) {
                                replaceTexture(renderer, currentTexture, equalizedSurface, rect);
                                SDL_DestroySurface(equalizedSurface);
                                isEqualized = true;
                                SDL_SetWindowTitle(window, "PixelLab - Equalized");
                            }
                        } else {
                            replaceTexture(renderer, currentTexture, originalGraySurface, rect);
                            isEqualized = false;
                            SDL_SetWindowTitle(window, "PixelLab - Grayscale");
                        }
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, currentTexture, NULL, &rect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(currentTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroySurface(originalGraySurface);
    SDL_Quit();

    return 0;
}