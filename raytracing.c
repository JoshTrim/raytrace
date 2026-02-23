#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 900
#define HEIGHT 600
#define DELAY 5000
#define COLOR_WHITE 0xFFFFFF

struct Circle {
  double x;
  double y;
  double r;
};

struct Color {
  int r;
  int g;
  int b;
  int a;
};

#define RED (struct Color){255, 0, 0, 0}
#define GREEN (struct Color){0, 255, 0, 0}
#define BLUE (struct Color){0, 0, 255, 0}

void FillCircle(SDL_Renderer *renderer, struct Circle circle,
                struct Color color) {

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  double radius_squared = pow(circle.r, 2);
  for (double x = circle.x - circle.r; x <= circle.x + circle.r; x++) {
    for (double y = circle.y - circle.r; y <= circle.y + circle.r; y++) {
      double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);
      if (distance_squared <= radius_squared) {
        SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
        SDL_RenderFillRect(renderer, &pixel);
      }
    }
  }
}

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

void draw(SDL_Renderer **renderer) {

  // draw
  struct Circle circle = {WIDTH / 2, HEIGHT / 2, 50};
  FillCircle(*renderer, circle, BLUE);
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

    // clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw(&renderer);

    SDL_RenderPresent(renderer);

    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
