#include "App.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Uso: " << argv[0] << " caminho_da_imagem" << std::endl;
    return 1;
  }
  
  App app;
  if (!app.init(argv[1])) {
    return 1;
  }

  app.run();
  return 0;
}
