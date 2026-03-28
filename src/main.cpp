#include "App.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

int main(int argc, char **argv) {
  App app;

  if (!app.init()) {
    return 1;
  }

  app.run();
  return 0;
}
