#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 900
#define HEIGHT 600
#define DELAY 5000

int checkenvbool(char *name) {
  char *env_p = getenv(name);
  if (env_p && strcmp(env_p, "1") == 0) {
    return 1;
  }
  return 0;
}

void debuginfo(int debug) {
  if (debug != 0) {
    printf("num video drivers: %d\n", SDL_GetNumVideoDrivers());
  }
}

int initsdl(SDL_Window **window, SDL_Renderer **renderer) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  *window =
      SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (!*window) {
    printf("window error: %s", SDL_GetError);
    return 1;
  }

  *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

  if (!*renderer) {
    printf("window error: %s", SDL_GetError);
    return 1;
  }

  return 0;
}

int main(void) {

  int DEBUG = checkenvbool("RT_DEBUG");

  debuginfo(DEBUG);

  int running = 1;
  SDL_Event e;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  int initialised = initsdl(&window, &renderer);
  if (initialised != 0) {
    exit(EXIT_FAILURE);
  }

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = 0;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
