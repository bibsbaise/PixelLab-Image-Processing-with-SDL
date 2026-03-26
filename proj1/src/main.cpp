#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

/*  
Para compilar: 
g++ src/main.cpp -o pixel_lab.exe -I"C:/Users/bibiz/Downloads/compvisual/SDL3/include" -I"C:/Users/bibiz/Downloads/compvisual/SDL3_image/include" -L"C:/Users/bibiz/Downloads/compvisual/SDL3/lib" -L"C:/Users/bibiz/Downloads/compvisual/SDL3_image/lib" -lSDL3 -lSDL3_image -mconsole
*/

/*
Para rodar:
.\pixel_lab.exe assets/teste.jpg
*/

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <cstdint>

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
    int totalPixels = converted->w * converted->h;

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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: programa caminho_da_imagem\n";
        return 1;
    }

    const char* imagePath = argv[1];

    SDL_Log("Antes do SDL_Init");

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

    SDL_Surface* graySurface = convertToGrayscale(loadedSurface);
    SDL_DestroySurface(loadedSurface);

    if (!graySurface) {
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
    "PixelLab - Grayscale",
    graySurface->w,
    graySurface->h,
    0
);

    if (!window) {
        SDL_Log("Erro ao criar janela: %s", SDL_GetError());
        SDL_DestroySurface(graySurface);
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Erro ao criar renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroySurface(graySurface);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, graySurface);
    SDL_DestroySurface(graySurface);

    if (!texture) {
        SDL_Log("Erro ao criar textura: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "Imagem aberta com sucesso!\n";

    SDL_FRect rect;
    rect.x = 0.0f;
    rect.y = 0.0f;
    SDL_GetTextureSize(texture, &rect.w, &rect.h);

    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, &rect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}