#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

struct WindowDeleter {
  void operator()(SDL_Window *ptr) const {
    if (ptr)
      SDL_DestroyWindow(ptr);
  }
};

struct RendererDeleter {
  void operator()(SDL_Renderer *ptr) const {
    if (ptr)
      SDL_DestroyRenderer(ptr);
  }
};

struct TextureDeleter {
  void operator()(SDL_Texture *ptr) const {
    if (ptr)
      SDL_DestroyTexture(ptr);
  }
};

struct SurfaceDeleter {
  void operator()(SDL_Surface *ptr) const {
    if (ptr)
      SDL_DestroySurface(ptr);
  }
};

struct TextEngineDeleter {
  void operator()(TTF_TextEngine *ptr) const {
    if (ptr)
      TTF_DestroyRendererTextEngine(ptr);
  }
};

struct FontDeleter {
  void operator()(TTF_Font *ptr) const {
    if (ptr)
      TTF_CloseFont(ptr);
  }
};

struct TextDeleter {
  void operator()(TTF_Text *ptr) const {
    if (ptr)
      TTF_DestroyText(ptr);
  }
};

// literalmente a representação de uma janela no sistema operacional
using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;

// atrelado a uma janela, nos permite desenhar nela
using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;

// representação de uma imagem na GPU, criada a partir de uma Surface
using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;

// representação de uma imagem na RAM, criada a partir de um arquivo ou manipulada pela CPU
using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

// usado para renderizar textos, criado a partir de uma fonte
using TextEnginePtr = std::unique_ptr<TTF_TextEngine, TextEngineDeleter>;

// representação de uma fonte, é criada a partir de um arquivo .ttf
using FontPtr = std::unique_ptr<TTF_Font, FontDeleter>;

// representação de um texto, criado a partir de uma string em uma text engine
using TextPtr = std::unique_ptr<TTF_Text, TextDeleter>;
