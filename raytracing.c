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
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define DELAY 5000
#define HYPOTENUSE sqrt((WIDTH * WIDTH) + (HEIGHT * HEIGHT))
#define COLOR_WHITE 0xFFFFFF
#define N_OBJECTS 5
#define MAX_RADIUS 50

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

struct Ray {
  double x_start, y_start;
  double x_end, y_end;
  double angle;
};

void DrawRay(struct Ray r, struct Color color, SDL_Renderer **renderer,
             int (*m)[WIDTH]) {
  // gonna be tricky - im drawing lines at the moment but need to check each
  // pixel
  SDL_SetRenderDrawColor(*renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawLine(*renderer, r.x_start, r.y_start, r.x_end, r.y_end);
}

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

// void draw(SDL_Renderer **renderer) {
//
//   // draw
//   struct Circle circle = {WIDTH / 2, HEIGHT / 2, 50};
//   FillCircle(*renderer, circle, BLUE);
// }

void rays(struct Circle circle, struct Color color, int n, int length,
          SDL_Renderer **renderer, int (*m)[WIDTH]) {
  double angle_step = (2 * M_PI) / n;
  int collide_object = 0;
  int collide_screen_edge = 0;
  for (int i = 0; i < n; i++) {

    if (!collide_object && !collide_screen_edge) {
      double angle = i * angle_step;
      struct Ray ray;
      ray.x_start = circle.x + (circle.r * cos(angle));
      ray.y_start = circle.y + (circle.r * sin(angle));
      ray.x_end = circle.x + (length * cos(angle));
      ray.y_end = circle.y + (length * sin(angle));

      DrawRay(ray, color, renderer, m);

      if (ray.x_end < 0 || ray.x_end > WIDTH)
        collide_screen_edge = 1;
      if (ray.y_end < 0 || ray.y_end > HEIGHT)
        collide_screen_edge = 1;

      collide_object = 0;
      collide_screen_edge = 0;
    }
  }
}

int euclidean_distance(struct Circle circle, int i, int j) {
  if (pow((i - circle.x), 2) + pow((j - circle.y), 2) <= pow(circle.r, 2)) {
    return 1;
  }
  return 0;
}

void draw_collision_matrix(int (*m)[WIDTH], struct Circle objects[N_OBJECTS]) {
  for (int x = 0; x < N_OBJECTS; x++) {

    struct Circle object = objects[x];

    // rows
    for (int i = 0; i < HEIGHT; i++) {
      // columns
      for (int j = 0; j < WIDTH; j++) {

        // circles
        for (x = 0; x < N_OBJECTS; x++) {
          int within_bounds = euclidean_distance(objects[i], i, j);
          if (within_bounds == 1) {
            m[i][j] = 1;
          }
        }
      }
    }
  }
}

int main(void) {

  srand((unsigned int)time(NULL));

  int DEBUG = checkenvbool("RT_DEBUG");

  debuginfo(DEBUG);

  // rows, columns
  int collision_matrix[HEIGHT][WIDTH] = {0};

  int running = 1;
  SDL_Event e;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  int initialised = initsdl(&window, &renderer);
  if (initialised != 0) {
    exit(EXIT_FAILURE);
  }

  // circles
  struct Circle circle = {200, 200, 80};
  struct Circle shadow = {650, 300, 120};

  struct Circle objects[N_OBJECTS];

  for (int i = 0; i < N_OBJECTS; i++) {
    int rand_x, rand_y, rand_r;

    rand_x = rand() % (WIDTH + 1);
    rand_y = rand() % (HEIGHT + 1);
    rand_r = rand() % (MAX_RADIUS + 1);

    struct Circle object = {rand_x, rand_y, rand_r};
    objects[i] = object;
  }

  while (running) {
    while (SDL_PollEvent(&e)) {

      if (e.type == SDL_QUIT) {
        running = 0;
      }

      if (e.type == SDL_MOUSEMOTION && e.motion.state != 0) {
        if (e.motion.x > 0 && e.motion.x < WIDTH)
          circle.x = e.motion.x;
        if (e.motion.y > 0 && e.motion.y < HEIGHT)
          circle.y = e.motion.y;
      }
    }

    // clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    rays(circle, RED, 360, HYPOTENUSE, &renderer, collision_matrix);
    FillCircle(renderer, circle, BLUE);

    for (int i = 0; i < N_OBJECTS; i++) {
      FillCircle(renderer, objects[i], GREEN);
    }
    // draw(&renderer);

    SDL_RenderPresent(renderer);

    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
