#include "pluto.h"

#include "SDL3_ttf/SDL_ttf.h"

#include "game_modules/log.h"
#include "game_modules/profiling.h"

ECS_COMPONENT_DECLARE (app_s);

ECS_COMPONENT_DECLARE (alpha_c);
ECS_COMPONENT_DECLARE (bounds_c);
ECS_COMPONENT_DECLARE (box_c);
ECS_COMPONENT_DECLARE (color_c);
ECS_COMPONENT_DECLARE (origin_c);

/******************************/
/*********** Hooks ************/
/******************************/

static void
alpha (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  alpha_c *alpha = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      alpha[i].value = 255u;
    }
}

static void
bounds (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  bounds_c *bounds = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      bounds[i].size = (SDL_FPoint){ 0.f, 0.f };
      bounds[i].b_can_be_scaled = false;
    }
}

static void
box (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  box_c *box = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      box[i].b_is_shown = true;
      box[i].b_is_filled = false;
      box[i].b_uses_color = false;
    }
}

static void
color (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  color_c *color = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      color[i].default_r = 255u;
      color[i].default_g = 255u;
      color[i].default_b = 255u;
      color[i].r = color[i].default_r;
      color[i].g = color[i].default_g;
      color[i].b = color[i].default_b;
      color[i].b_should_reset = true;
    }
}

static void
origin (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  origin_c *origin = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      origin[i].position = (SDL_FPoint){ 0.f, 0.f };
      origin[i].position_callback = NULL;
      origin[i].b_is_center = false;
      origin[i].b_can_be_scaled = false;
      origin[i].b_is_screen_based = false;
    }
}

/******************************/
/********** Systems ***********/
/******************************/

static void
system_box_draw (ecs_iter_t *it)
{
  box_c *box = ecs_field (it, box_c, 0);

  bounds_c *bounds = ecs_field (it, bounds_c, 1);
  origin_c *origin = ecs_field (it, origin_c, 2);

  const app_s *app = ecs_singleton_get (it->world, app_s);

  Sint8 alpha_id = 3;
  alpha_c *alpha_opt = NULL;
  if (ecs_field_is_set (it, alpha_id) == true)
    {
      alpha_opt = ecs_field (it, alpha_c, alpha_id);
    }
  Sint8 color_id = 4;
  color_c *color_opt = NULL;
  if (ecs_field_is_set (it, color_id) == true)
    {
      color_opt = ecs_field (it, color_c, color_id);
    }

  for (Sint32 i = 0; i < it->count; i++)
    {
      if (box[i].b_is_shown == false)
        {
          continue;
        }
      SDL_FRect dest = (SDL_FRect){ origin[i].position.x, origin[i].position.y,
                                    bounds[i].size.x, bounds[i].size.y };

      Uint8 box_r = 255u;
      Uint8 box_g = 255u;
      Uint8 box_b = 255u;
      Uint8 box_alpha = 255u;
      if (box[i].b_uses_color == true)
        {
          if (color_opt != NULL && &color_opt[i] != NULL)
            {
              box_r = color_opt[i].r;
              box_g = color_opt[i].g;
              box_b = color_opt[i].b;
            }
          if (alpha_opt != NULL && &alpha_opt[i] != NULL)
            {
              box_alpha = alpha_opt[i].value;
            }
        }
      if (origin[i].b_can_be_scaled == true)
        {
          dest.x *= app->scale;
          dest.y *= app->scale;
        }
      if (bounds[i].b_can_be_scaled == true)
        {
          dest.w *= app->scale;
          dest.h *= app->scale;
        }
      if (origin[i].b_is_center == true)
        {
          dest.x -= dest.w / 2;
          dest.y -= dest.h / 2;
        }
      SDL_SetRenderDrawColor (app->rend, box_r, box_g, box_b, box_alpha);
      if (box[i].b_is_filled == true)
        {
          SDL_RenderFillRect (app->rend, &dest);
          continue;
        }
      SDL_RenderRect (app->rend, &dest);
    }
}

/******************************/
/*********** Init *************/
/******************************/

static void
init_pluto_systems (ecs_world_t *ecs)
{
  {
    ecs_entity_t ent = ecs_entity (
        ecs,
        { .name = "system_box_draw_objects",
          .add = ecs_ids (ecs_dependson (ecs_lookup (ecs, "render_phase"))) });
    ecs_query_desc_t query
        = { .terms = { { .id = ecs_id (box_c) },
                       { .id = ecs_id (bounds_c) },
                       { .id = ecs_id (origin_c) },
                       { .id = ecs_id (alpha_c), .oper = EcsOptional },
                       { .id = ecs_id (color_c), .oper = EcsOptional } } };
    ecs_system (
        ecs, { .entity = ent, .query = query, .callback = system_box_draw });
  }
}


static void
init_pluto_phases (ecs_world_t *ecs)
{

  ecs_entity_t reset_phase = ecs_new_w_id (ecs, EcsPhase);
  ecs_set_name (ecs, reset_phase, "reset_phase");

  ecs_entity_t input_phase = ecs_new_w_id (ecs, EcsPhase);
  ecs_set_name (ecs, input_phase, "input_phase");

  ecs_entity_t callback_phase = ecs_new_w_id (ecs, EcsPhase);
  ecs_set_name (ecs, callback_phase, "callback_phase");

  ecs_entity_t pre_render_phase = ecs_new_w_id (ecs, EcsPhase);
  ecs_set_name (ecs, pre_render_phase, "pre_render_phase");

  ecs_entity_t render_phase = ecs_new_w_id (ecs, EcsPhase);
  ecs_set_name (ecs, render_phase, "render_phase");

  ecs_add_pair (ecs, reset_phase, EcsDependsOn, EcsPreUpdate);
  ecs_add_pair (ecs, input_phase, EcsDependsOn, reset_phase);
  ecs_add_pair (ecs, callback_phase, EcsDependsOn, EcsOnUpdate);
  ecs_add_pair (ecs, pre_render_phase, EcsDependsOn, callback_phase);
  ecs_add_pair (ecs, render_phase, EcsDependsOn, pre_render_phase);
}

static void
init_pluto_hooks (ecs_world_t *ecs)
{
  ecs_type_hooks_t alpha_hooks = { .ctor = alpha };
  ecs_set_hooks_id (ecs, ecs_id (alpha_c), &alpha_hooks);

  ecs_type_hooks_t bounds_hooks = { .ctor = bounds };
  ecs_set_hooks_id (ecs, ecs_id (bounds_c), &bounds_hooks);

  ecs_type_hooks_t box_hooks = { .ctor = box };
  ecs_set_hooks_id (ecs, ecs_id (box_c), &box_hooks);

  ecs_type_hooks_t color_hooks = { .ctor = color };
  ecs_set_hooks_id (ecs, ecs_id (color_c), &color_hooks);

  ecs_type_hooks_t origin_hooks = { .ctor = origin };
  ecs_set_hooks_id (ecs, ecs_id (origin_c), &origin_hooks);
}

static SDL_Renderer *
init_pluto_sdl (ecs_world_t *ecs, const SDL_Point window_size)
{
  app_s *app = ecs_singleton_ensure (ecs, app_s);
  SDL_SetHint ("SDL_WINDOWS_DPI_AWARENESS", "1");
  SDL_Init (SDL_INIT_VIDEO);
  TTF_Init ();
  app->win = SDL_CreateWindow ("Console", window_size.x, window_size.y,
                               SDL_WINDOW_RESIZABLE);
  app->rend = SDL_CreateRenderer (app->win, NULL);
  SDL_SetRenderVSync (app->rend, true);
  SDL_SetHint (SDL_HINT_GPU_DRIVER, "vulkan");
  SDL_SetHint (SDL_HINT_RENDER_GPU_DEBUG, "1");
  SDL_SetRenderDrawBlendMode (app->rend, SDL_BLENDMODE_BLEND);

  Sint32 compiled_v = SDL_VERSION;
  Sint32 linked_v = SDL_GetVersion ();
  log_debug (
      DEBUG_LOG_NONE,
      "Binary compiled with SDL %d.%d.%d. Binary linked against SDL %d.%d.%d.",
      SDL_VERSIONNUM_MAJOR (compiled_v), SDL_VERSIONNUM_MINOR (compiled_v),
      SDL_VERSIONNUM_MICRO (compiled_v), SDL_VERSIONNUM_MAJOR (linked_v),
      SDL_VERSIONNUM_MINOR (linked_v), SDL_VERSIONNUM_MICRO (linked_v));
  app->scale = 1.f;
  app->frame_data = SDL_calloc (1, sizeof (struct frame_data));
  return app->rend;
}

SDL_Renderer *
init_pluto (ecs_world_t *ecs, const SDL_Point window_size)
{
  ECS_COMPONENT_DEFINE (ecs, app_s);
  ECS_COMPONENT_DEFINE (ecs, alpha_c);
  ECS_COMPONENT_DEFINE (ecs, bounds_c);
  ECS_COMPONENT_DEFINE (ecs, box_c);
  ECS_COMPONENT_DEFINE (ecs, color_c);
  ECS_COMPONENT_DEFINE (ecs, origin_c);
  SDL_Renderer *rend = init_pluto_sdl (ecs, window_size);
  init_pluto_hooks (ecs);
  init_pluto_phases(ecs);
  init_pluto_systems (ecs);
  return rend;
}
