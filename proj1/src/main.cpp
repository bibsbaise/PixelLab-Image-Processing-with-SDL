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

// VAR GLOBAL
int vetorIntensidade[256] = { 0 };
//


void calcular_histograma(SDL_Renderer *renderer, MyImage *image)
{
  SDL_Log(">>> calcular_histograma()");

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< calcular_histograma()");
    return;
  }

  if (!image || !image->surface)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL ou image->surface == NULL).");
    SDL_Log("<<< calcular_histograma()");
    return;
  }

  // Resetar o vetor de intensidades
  for (int i = 0; i < 256; ++i)
  {
    vetorIntensidade[i] = 0;
  }

  // Para acessar os pixels de uma superfície, precisamos chamar essa função.
  SDL_LockSurface(image->surface);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(image->surface->format);
  const size_t pixelCount = image->surface->w * image->surface->h;

  Uint32 *pixels = (Uint32 *)image->surface->pixels;
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;
  Uint8 a = 0;

  for (size_t i = 0; i < pixelCount; ++i)
  {
    SDL_GetRGBA(pixels[i], format, NULL, &r, &g, &b, &a);

    if (r == g && g == b)
    {
      vetorIntensidade[r]++;
    }
  }



  // Após manipularmos os pixels da superfície, liberamos a superfície.
  SDL_UnlockSurface(image->surface);

  // Atualizamos a textura a ser renderizada pelo SDL_Renderer, com base no
  // novo conteúdo da superfície.
  SDL_DestroyTexture(image->texture);
  image->texture = SDL_CreateTextureFromSurface(renderer, image->surface);
  SDL_Log("<<< calcular_histograma()");
}

// =====================

float calcularMediaHistograma(int vetorIntensidade[256])
{
  int soma = 0;
  int totalPixels = 0;

  for (int i = 0; i < 256; ++i)
  {
    soma += vetorIntensidade[i] * i; // multiplicamos a quantidade de pixels pela intensidade correspondente
    totalPixels += vetorIntensidade[i]; // somamos a quantidade total de pixels
  }

  if (totalPixels == 0)
    return 0.0f; // evitar divisão por zero

  return (float)soma / totalPixels; // calculamos a média dividindo a soma pelo total de pixels
}

int classificarBrilho(float media)
{
  if (media < 85.0f)
    return 0; // imagem escura
  else if (media < 170.0f)
    return 1; // imagem média
  else
    return 2; // imagem clara
}

// ==========================

float calcularDesvioPadrao(int vetorIntensidade[256], float media)
{
  int totalPixels = 0;
  float somaQuadrados = 0.0f;

  for (int i = 0; i < 256; ++i)
  {
    totalPixels += vetorIntensidade[i]; // somamos a quantidade total de pixels
    somaQuadrados += vetorIntensidade[i] * (i - media) * (i - media); // somamos o quadrado da diferença entre a intensidade e a média, multiplicado pela quantidade de pixels daquela intensidade
  }

  if (totalPixels == 0)
    return 0.0f; // evitar divisão por zero

  return sqrtf(somaQuadrados / totalPixels); // calculamos o desvio padrão dividindo a soma dos quadrados pelo total de pixels e tirando a raiz quadrada
}

int classificarContraste(float desvioPadrao)
{
  if (desvioPadrao < 50.0f)
    return 0; // baixo contraste
  else if (desvioPadrao < 100.0f)
    return 1; // contraste médio
  else
    return 2; // alto contraste
}



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
