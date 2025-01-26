#ifndef PLUTO_MODULE_H
#define PLUTO_MODULE_H

#include "SDL3/SDL.h"
#include "flecs.h"

#include "game_modules/render_target.h"

typedef struct singleton_app
{
  SDL_Window *win;
  SDL_Point window_size;
  SDL_Point window_cell_dims;
  bool b_is_fullscreen_presentation;
  SDL_Renderer *rend;
  struct game *game;
  struct screen *screen;
  struct frame_data *frame_data;
  struct sprite_atlas *atlas;
  struct text_man *text_man;
  struct input_man *input_man;
  ecs_entity_t current_scene;
  dict_render_target_t rts;
  float scale;
} app_s;

typedef struct component_alpha
{
  Uint8 value;
} alpha_c;

typedef struct component_bounds
{
  SDL_FPoint size;
  bool b_can_be_scaled;
} bounds_c;

typedef struct component_box
{
  bool b_is_shown;
  bool b_is_filled;
  bool b_uses_color;
} box_c;

typedef struct component_color
{
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 default_r;
  Uint8 default_g;
  Uint8 default_b;
  bool b_should_reset;
} color_c;

typedef struct component_origin
{
  SDL_FPoint position;
  void (*position_callback) (ecs_world_t *, SDL_FPoint *);
  bool b_is_center;
  bool b_can_be_scaled;
  bool b_is_screen_based;
} origin_c;

SDL_Renderer * init_pluto(ecs_world_t *ecs, const SDL_Point window_size);

#endif /* PLUTO_MODULE_H */
