#ifndef PLUTO_MODULE_H
#define PLUTO_MODULE_H

#include "SDL3/SDL.h"
#include "flecs.h"
#include "m-array.h"
#include "m-list.h"
#include "m-string.h"

#include "container_types.h"

#include "animation.h"
#include "sprite_atlas.h"
#include "render_target.h"

extern ECS_COMPONENT_DECLARE (core_s);

extern ECS_COMPONENT_DECLARE (array_c);
extern ECS_COMPONENT_DECLARE (alpha_c);
extern ECS_COMPONENT_DECLARE (anim_player_c);
extern ECS_COMPONENT_DECLARE (bounds_c);
extern ECS_COMPONENT_DECLARE (box_c);
extern ECS_COMPONENT_DECLARE (cache_c);
extern ECS_COMPONENT_DECLARE (click_c);
extern ECS_COMPONENT_DECLARE (color_c);
extern ECS_COMPONENT_DECLARE (drag_c);
extern ECS_COMPONENT_DECLARE (hover_c);
extern ECS_COMPONENT_DECLARE (index_c);
extern ECS_COMPONENT_DECLARE (layer_c);
extern ECS_COMPONENT_DECLARE (mat2d_c);
extern ECS_COMPONENT_DECLARE (mat3d_c);
extern ECS_COMPONENT_DECLARE (margins_c);
extern ECS_COMPONENT_DECLARE (movement_c);
extern ECS_COMPONENT_DECLARE (ngrid_c);
extern ECS_COMPONENT_DECLARE (origin_c);
extern ECS_COMPONENT_DECLARE (pattern_c);
extern ECS_COMPONENT_DECLARE (render_target_c);
extern ECS_COMPONENT_DECLARE (resize_c);
extern ECS_COMPONENT_DECLARE (scroll_to_c);
extern ECS_COMPONENT_DECLARE (sprite_c);
extern ECS_COMPONENT_DECLARE (text_c);
extern ECS_COMPONENT_DECLARE (visibility_c);

#define MARGIN_HANDLE_NONE 0
#define MARGIN_HANDLE_TOP_EDGE 1
#define MARGIN_HANDLE_TOP_LEFT_CORNER 2
#define MARGIN_HANDLE_LEFT_EDGE 3
#define MARGIN_HANDLE_BOT_LEFT_CORNER 4
#define MARGIN_HANDLE_BOT_EDGE 5
#define MARGIN_HANDLE_BOT_RIGHT_CORNER 6
#define MARGIN_HANDLE_RIGHT_EDGE 7
#define MARGIN_HANDLE_TOP_RIGHT_CORNER 8
#define MARGIN_HANDLE_TYPE_MAX 9

ARRAY_DEF (arr_entity, ecs_entity_t, M_BASIC_OPLIST)
ARRAY_DEF (mat2d_entity, arr_entity_t, M_ARRAY_OPLIST (arr_entity))
ARRAY_DEF (mat3d_entity, mat2d_entity_t, M_ARRAY_OPLIST (mat2d_entity))

DICT_DEF2 (dict_string_to_query_ptr, string_t, M_STRING_OPLIST, ecs_query_t *,
           M_PTR_OPLIST)

struct pluto_input_data
{
  bool b_is_dragging_widget;
  bool b_is_resizing_widget;
  bool b_is_moving_camera;
  bool b_block_movement_hold;
};

struct pluto_core_params
{
  const char *window_name;
  SDL_Point default_win_size;
  SDL_InitFlags init_flags;
  SDL_WindowFlags window_flags;
  float default_user_scaling;
  const char *gpu_driver_hint;
  bool b_should_debug_GPU;
  bool b_is_DPI_aware;
  bool b_has_logical_size;
  SDL_RendererLogicalPresentation logical_presentation_mode;
  SDL_BlendMode renderer_blend_mode;
  struct pluto_input_data input_data;
};

/******************************/
/******** Components **********/
/******************************/

typedef struct singleton_core
{
  SDL_Window *win;
  SDL_Point window_size;
  SDL_Point logical_size;
  bool b_is_fullscreen_presentation;
  SDL_Renderer *rend;
  struct screen *screen;
  struct frame_data *frame_data;
  struct sprite_atlas *atlas;
  struct text_man *text_man;
  struct input_man *input_man;
  struct rts_manager *rts;
  float scale;
  SDL_FPoint scroll_value;
} core_s;

typedef struct component_alpha
{
  Uint8 value;
} alpha_c;

typedef struct component_anim_player
{
  dict_string_anim_pose_t poses;
  Uint8 current_tick;
  string_t control_pose;
  Sint32 control_direction;
  SDL_Point current_frame;
  SDL_FRect subsection;
} anim_player_c;

typedef struct component_array
{
  arr_entity_t content;
} array_c;

typedef struct component_bounds
{
  SDL_FPoint size;
  bool b_can_be_scaled;
  SDL_FPoint (*size_callback) (ecs_entity_t self, ecs_world_t *world);
  float (*width_callback) (ecs_entity_t self, ecs_world_t *world);
  float (*height_callback) (ecs_entity_t self, ecs_world_t *world);
} bounds_c;

typedef struct component_box
{
  bool b_is_shown;
  bool b_is_filled;
  bool b_uses_color;
} box_c;

typedef struct component_cache
{
  bool b_should_regenerate;
  bool b_should_always_regen;
  string_t cache_name;
} cache_c;

typedef struct component_click
{
  void (*callback) (void *);
  Uint16 callback_delay;
  Uint8 toggled_r;
  Uint8 toggled_g;
  Uint8 toggled_b;
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

typedef struct component_drag
{
  bool b_state;
  Uint8 toggled_r;
  Uint8 toggled_g;
  Uint8 toggled_b;
} drag_c;

typedef struct component_hover
{
  bool b_state;
  Uint8 toggled_r;
  Uint8 toggled_g;
  Uint8 toggled_b;
} hover_c;

typedef struct component_index
{
  Sint32 x;
  Sint32 y;
  Sint32 z;
} index_c;

typedef struct component_layer
{
  Sint32 value;
} layer_c;

typedef struct component_matrice2d
{
  mat2d_entity_t content;
} mat2d_c;

typedef struct component_matrice3d
{
  mat3d_entity_t content;
} mat3d_c;

typedef struct component_margins
{
  struct margins_ngrid value;
  Uint8 corner_r;
  Uint8 corner_g;
  Uint8 corner_b;
  Uint8 corner_a;
  Uint8 edge_r;
  Uint8 edge_g;
  Uint8 edge_b;
  Uint8 edge_a;
  Sint32 current_handle;
  bool b_is_shown;
} margins_c;

typedef struct component_movement
{
  SDL_Point delta;
  Sint32 default_cooldown;
  Sint32 cooldown;
} movement_c;

typedef struct component_ngrid
{
  string_t name;
  bool b_is_shown;
  bool b_uses_color;
  bool b_tiled_edges;
} ngrid_c;

typedef struct component_origin
{
  SDL_FPoint world;
  SDL_FPoint relative;
  SDL_FPoint (*relative_callback) (ecs_entity_t ent, ecs_world_t *);
  float (*relative_x_callback) (ecs_entity_t ent, ecs_world_t *);
  float (*relative_y_callback) (ecs_entity_t ent, ecs_world_t *);
  bool b_is_center;
  bool b_can_be_scaled;
  bool b_is_screen_based;
} origin_c;

typedef struct component_pattern
{
  string_t name;
  bool b_is_shown;
  bool b_uses_color;
} pattern_c;

typedef struct component_render_target
{
  string_t name;
  bool b_is_shown;
  bool b_uses_color;
} render_target_c;

typedef struct component_resize
{
  bool b_state;
  bool from[MARGIN_HANDLE_TYPE_MAX];
  Uint8 toggled_r;
  Uint8 toggled_g;
  Uint8 toggled_b;
  Uint8 toggled_a;
} resize_c;

typedef struct component_scroll_to
{
  float strength;
} scroll_to_c;

typedef struct component_sprite
{
  string_t name;
  SDL_FPoint offset;
  bool b_is_shown;
  bool b_uses_color;
  bool b_overrides_bounds;
  SDL_FPoint over_size;
} sprite_c;

typedef struct component_text
{
  string_t content;
  void (*content_binding) (ecs_entity_t, ecs_world_t *, string_t);
  Sint32 align_h;
  Sint32 align_v;
  Uint8 font_size;
  bool b_is_shown;
  bool b_uses_color;
} text_c;

typedef struct component_visibility
{
  bool b_state;
  bool (*state_binding) (ecs_entity_t, ecs_world_t *);
} visibility_c;

/******************************/
/********* Core API ***********/
/******************************/

/* Call this at the start of your application. It deals with SDL and other
 * required constructs and returns a pointer to an ecs singleton containing
 * the core framework functionality. */
core_s *init_pluto (ecs_world_t *ecs, struct pluto_core_params *params);

/* Implement these callbacks in your application. Only mouse and keyboard are
 * supported for now.*/
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

SDL_FPoint get_mouse_position (ecs_entity_t ent, ecs_world_t *ecs);

#endif /* PLUTO_MODULE_H */
