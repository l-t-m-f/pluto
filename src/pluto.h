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

typedef struct component_click
{
  void (*callback) (void *);
  Uint16 callback_delay;
  bool b_state;
} click_c;

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

typedef struct component_hover
{
  bool b_state;
  Uint8 toggled_r;
  Uint8 toggled_g;
  Uint8 toggled_b;
} hover_c;

typedef struct component_origin
{
  SDL_FPoint world;
  SDL_FPoint relative;
  void (*position_callback) (ecs_world_t *, SDL_FPoint *);
  bool b_is_center;
  bool b_can_be_scaled;
  bool b_is_screen_based;
} origin_c;

typedef struct component_text
{
  string_t content;
  Sint32 align_h;
  Sint32 align_v;
  Uint8 font_size;
  bool b_is_shown;
  bool b_uses_color;
} text_c;

app_s *init_pluto (ecs_world_t *ecs, SDL_Point window_size);

extern void handle_key_press (struct input_man *input_man, SDL_Scancode key,
                              void *param);
extern void handle_key_release (struct input_man *input_man, SDL_Scancode key,
                                void *param);
extern void handle_key_hold (struct input_man *input_man, SDL_Scancode key,
                             void *param);
extern void handle_mouse_press (struct input_man *input_man, SDL_FPoint pos,
                                Uint8 button, void *param);
extern void handle_mouse_release (struct input_man *input_man, SDL_FPoint pos,
                                  Uint8 button, void *param);
extern void handle_mouse_hold (struct input_man *input_man, SDL_FPoint pos,
                               Uint8 button, void *param);
extern void handle_mouse_motion (struct input_man *input_man, SDL_FPoint pos,
                                 SDL_FPoint rel, void *param);

#endif /* PLUTO_MODULE_H */
