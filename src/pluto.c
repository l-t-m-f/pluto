#include "pluto.h"

#include "SDL3_ttf/SDL_ttf.h"

#include "game_modules/input_man.h"
#include "game_modules/log.h"
#include "game_modules/profiling.h"
#include "game_modules/sprite_atlas.h"
#include "game_modules/text_man.h"

ECS_COMPONENT_DECLARE (core_s);

ECS_COMPONENT_DECLARE (alpha_c);
ECS_COMPONENT_DECLARE (anim_player_c);
ECS_COMPONENT_DECLARE (bounds_c);
ECS_COMPONENT_DECLARE (box_c);
ECS_COMPONENT_DECLARE (click_c);
ECS_COMPONENT_DECLARE (color_c);
ECS_COMPONENT_DECLARE (hover_c);
ECS_COMPONENT_DECLARE (origin_c);
ECS_COMPONENT_DECLARE (sprite_c);
ECS_COMPONENT_DECLARE (text_c);

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
anim_player (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  anim_player_c *anim_player = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      anim_player[i].current_tick = 0u;
      anim_player[i].current_frame = (SDL_Point){ 0, 0 };
      anim_player[i].control_pose = "";
      anim_player[i].control_direction = 0;
      anim_player[i].subsection = (SDL_FRect){ 0.f, 0.f };
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
click (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  click_c *click = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      click[i].callback = NULL;
      click[i].callback_delay = 0u;
      click[i].toggled_r = 125u;
      click[i].toggled_g = 255u;
      click[i].toggled_b = 0u;
      click[i].b_state = false;
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
hover (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  hover_c *hover = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      hover[i].b_state = false;
      hover[i].toggled_r = 0u;
      hover[i].toggled_g = 125u;
      hover[i].toggled_b = 125u;
    }
}

static void
origin (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  origin_c *origin = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      origin[i].world = (SDL_FPoint){ 0.f, 0.f };
      origin[i].relative = (SDL_FPoint){ 0.f, 0.f };
      origin[i].relative_callback = NULL;
      origin[i].b_is_center = false;
      origin[i].b_can_be_scaled = false;
      origin[i].b_is_screen_based = false;
    }
}

static void
sprite (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  sprite_c *sprite = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      sprite[i].b_is_shown = true;
      sprite[i].b_uses_color = false;
      sprite[i].render_type = SPRITE_RENDER_TYPE_DEFAULT;
      sprite[i].offset = (SDL_FPoint){ 0.f, 0.f };
      sprite[i].b_overrides_bounds = false;
      sprite[i].over_size = (SDL_FPoint){ 0.f, 0.f };
      sprite[i].b_should_cache = false;
      sprite[i].b_should_regenerate = false;
    }
}

static void
text (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  text_c *text = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      text[i].align_h = TEXT_ALIGN_H_LEFT;
      text[i].align_v = TEXT_ALIGN_V_TOP;
      text[i].font_size = 5u;
      text[i].b_is_shown = true;
      text[i].b_uses_color = false;
    }
}

/******************************/
/****** System callbacks ******/
/******************************/

SDL_FPoint
get_mouse_position (ecs_world_t *corestate)
{
  const core_s *core = ecs_singleton_get (corestate, core_s);
  return core->input_man->mouse.position.window;
}

/******************************/
/********** Systems ***********/
/******************************/

static void
system_anim_player_advance (ecs_iter_t *it)
{
  anim_player_c *anim_player = ecs_field (it, anim_player_c, 0);

  for (Sint32 i = 0; i < it->count; i++)
    {
      string_t temp;
      string_init_set_str (temp, anim_player[i].control_pose);
      struct anim_pose *pose
          = *dict_string_anim_pose_get (anim_player[i].poses, temp);

      struct anim_flipbook *flipbook = *dict_sint32_anim_flipbook_get (
          pose->directions, anim_player[i].control_direction);

      if (anim_player[i].current_tick >= flipbook->play_speed)
        {
          if (anim_player[i].current_frame.x >= flipbook->frame_count.x - 1)
            {
              if (anim_player[i].current_frame.y
                  >= flipbook->frame_count.y - 1)
                {
                  anim_player[i].current_frame = (SDL_Point){ 0, 0 };
                }
              else
                {
                  anim_player[i].current_frame.x = 0;
                  anim_player[i].current_frame.y++;
                }
            }
          else
            {
              anim_player[i].current_frame.x++;
            }
          anim_player[i].current_tick = 0;
        }
      else
        {
          anim_player[i].current_tick++;
        }

      string_clear (temp);
    }
}

static void
system_anim_player_assign_sprite_name (ecs_iter_t *it)
{
  anim_player_c *anim_player = ecs_field (it, anim_player_c, 0);
  sprite_c *sprite = ecs_field (it, sprite_c, 1);

  for (Sint32 i = 0; i < it->count; i++)
    {
      string_t name;
      string_init_set_str (name, anim_player[i].control_pose);
      struct anim_pose *pose
          = *dict_string_anim_pose_get (anim_player[i].poses, name);
      string_clear (name);

      struct anim_flipbook *flipbook = *dict_sint32_anim_flipbook_get (
          pose->directions, anim_player[i].control_direction);
      string_init_set (sprite[i].name, flipbook->name);
    }
}

static void
system_anim_player_set_subsection (ecs_iter_t *it)
{
  anim_player_c *anim_player = ecs_field (it, anim_player_c, 0);

  for (Sint32 i = 0; i < it->count; i++)
    {
      string_t name;
      string_init_set_str (name, anim_player[i].control_pose);
      struct anim_pose *pose
          = *dict_string_anim_pose_get (anim_player[i].poses, name);
      string_clear (name);
      struct anim_flipbook *flipbook = *dict_sint32_anim_flipbook_get (
          pose->directions, anim_player[i].control_direction);

      SDL_FRect new_subsection = {
        .x = (float)anim_player[i].current_frame.x * flipbook->frame_size.x,
        .y = (float)anim_player[i].current_frame.y * flipbook->frame_size.y,
        .w = flipbook->frame_size.x,
        .h = flipbook->frame_size.y
      };
      anim_player[i].subsection = new_subsection;
    }
}

static void
system_box_draw (ecs_iter_t *it)
{
  box_c *box = ecs_field (it, box_c, 0);

  bounds_c *bounds = ecs_field (it, bounds_c, 1);
  origin_c *origin = ecs_field (it, origin_c, 2);

  const core_s *core = ecs_singleton_get (it->world, core_s);

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
      SDL_FRect dest = (SDL_FRect){ origin[i].world.x, origin[i].world.y,
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
          dest.x *= core->scale;
          dest.y *= core->scale;
        }
      if (bounds[i].b_can_be_scaled == true)
        {
          dest.w *= core->scale;
          dest.h *= core->scale;
        }
      if (origin[i].b_is_center == true)
        {
          dest.x -= dest.w / 2;
          dest.y -= dest.h / 2;
        }
      SDL_SetRenderDrawColor (core->rend, box_r, box_g, box_b, box_alpha);
      if (box[i].b_is_filled == true)
        {
          SDL_RenderFillRect (core->rend, &dest);
          continue;
        }
      SDL_RenderRect (core->rend, &dest);
    }
}

static void
system_click_toggle (ecs_iter_t *it)
{
  click_c *click = ecs_field (it, click_c, 0);

  hover_c *hover = ecs_field (it, hover_c, 1);

  const core_s *core = ecs_singleton_get (it->world, core_s);

  for (Sint32 i = 0; i < it->count; i++)
    {
      click[i].b_state = false;
      if (hover[i].b_state == false)
        {
          continue;
        }
      if (core->input_man->mouse.b_is_lmb_down == false)
        {
          continue;
        }
      click[i].b_state = true;
    }
}

static void
system_color_set (ecs_iter_t *it)
{
  color_c *color = ecs_field (it, color_c, 0);
  Sint8 hover_id = 1;
  hover_c *hover = NULL;
  if (ecs_field_is_set (it, hover_id) == true)
    {
      hover = ecs_field (it, hover_c, hover_id);
    }
  Sint8 click_id = 2;
  click_c *click = NULL;
  if (ecs_field_is_set (it, click_id) == true)
    {
      click = ecs_field (it, click_c, click_id);
    }
  for (Sint32 i = 0; i < it->count; i++)
    {
      if (click != NULL && &click[i] != NULL)
        {
          if (click[i].b_state == true)
            {
              color[i].r = click[i].toggled_r;
              color[i].g = click[i].toggled_g;
              color[i].b = click[i].toggled_b;
              continue;
            }
        }
      if (hover != NULL && &hover[i] != NULL)
        {
          if (hover[i].b_state == true)
            {
              color[i].r = hover[i].toggled_r;
              color[i].g = hover[i].toggled_g;
              color[i].b = hover[i].toggled_b;
              continue;
            }
        }
      color[i].r = color[i].default_r;
      color[i].g = color[i].default_g;
      color[i].b = color[i].default_b;
    }
}

static void
system_hover_toggle (ecs_iter_t *it)
{
  hover_c *hover = ecs_field (it, hover_c, 0);

  origin_c *origin = ecs_field (it, origin_c, 1);
  bounds_c *bounds = ecs_field (it, bounds_c, 2);

  const core_s *core = ecs_singleton_get (it->world, core_s);

  for (Sint32 i = 0; i < it->count; i++)
    {
      SDL_FRect dest = (SDL_FRect){ .x = origin[i].world.x,
                                    .y = origin[i].world.y,
                                    .w = bounds[i].size.x,
                                    .h = bounds[i].size.y };
      SDL_FPoint mouse_position = core->input_man->mouse.position.window;
      if (origin[i].b_can_be_scaled == true)
        {
          dest.x *= core->scale;
          dest.y *= core->scale;
        }
      if (bounds[i].b_can_be_scaled == true)
        {
          dest.w *= core->scale;
          dest.h *= core->scale;
        }
      if (origin[i].b_is_center == true)
        {
          dest.x -= dest.w / 2;
          dest.y -= dest.h / 2;
        }
      SDL_FRect collider = (SDL_FRect){
        .x = dest.x, .y = dest.y, .w = dest.x + dest.w, .h = dest.y + dest.h
      };

      hover[i].b_state = false;

      if ((mouse_position.x >= collider.x && mouse_position.x <= collider.w)
          && (mouse_position.y >= collider.y
              && mouse_position.y <= collider.h))
        {
          hover[i].b_state = true;
        }
    }
}

static void
system_origin_bind_relative (ecs_iter_t *it)
{
  origin_c *origin = ecs_field (it, origin_c, 0);
  for (Sint32 i = 0; i < it->count; i++)
    {
      if (origin[i].relative_callback == NULL)
        {
          continue;
        }
      origin[i].relative = origin[i].relative_callback (it->world);
    }
}

static void
system_origin_calc_world (ecs_iter_t *it)
{
  origin_c *origin = ecs_field (it, origin_c, 0);
  Sint8 p_origin_id = 1;
  origin_c *p_origin = NULL;
  if (ecs_field_is_set (it, p_origin_id) == true)
    {
      p_origin = ecs_field (it, origin_c, p_origin_id);
    }

  for (Sint32 i = 0; i < it->count; i++)
    {
      if (p_origin != NULL && &p_origin[i] != NULL)
        {
          origin[i].world.x = p_origin->world.x + origin[i].relative.x;
          origin[i].world.y = p_origin->world.y + origin[i].relative.y;
        }
      else
        {
          origin[i].world.x = origin[i].relative.x;
          origin[i].world.y = origin[i].relative.y;
        }
    }
}

static void
system_sprite_draw (ecs_iter_t *it)
{
  sprite_c *sprite = ecs_field (it, sprite_c, 0);

  bounds_c *bounds = ecs_field (it, bounds_c, 1);
  origin_c *origin = ecs_field (it, origin_c, 2);
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
  Sint8 anim_player_id = 5;
  anim_player_c *anim_player_opt = NULL;
  if (ecs_field_is_set (it, anim_player_id) == true)
    {
      anim_player_opt = ecs_field (it, anim_player_c, anim_player_id);
    }

  core_s *core = ecs_get_mut (it->world, ecs_id (core_s), core_s);

  for (Sint32 i = 0; i < it->count; i++)
    {
      if (ecs_has_pair (it->world, it->entities[i],
                        ecs_lookup (it->world, "scene"), EcsAny)
              == true
          && ecs_has_pair (it->world, it->entities[i],
                           ecs_lookup (it->world, "scene"),
                           core->current_scene)
                 == false)
        {
          continue;
        }
      if (sprite[i].b_is_shown == false)
        {
          continue;
        }
      SDL_FRect dest = { .x = origin[i].world.x, .y = origin[i].world.y };

      if (origin[i].b_can_be_scaled == true)
        {
          dest.x *= core->scale;
          dest.y *= core->scale;
        }
      if (sprite[i].b_overrides_bounds == true)
        {

          if (bounds[i].b_can_be_scaled == true)
            {
              dest.x += (sprite[i].offset.x * core->scale);
              dest.y += (sprite[i].offset.y * core->scale);
            }
          else
            {
              dest.x += sprite[i].offset.x;
              dest.y += sprite[i].offset.y;
            }
          dest.w = sprite[i].over_size.x;
          dest.h = sprite[i].over_size.y;
        }
      else
        {
          dest.w = bounds[i].size.x;
          dest.h = bounds[i].size.y;
        }
      if (bounds[i].b_can_be_scaled == true)
        {
          dest.w *= core->scale;
          dest.h *= core->scale;
        }
      if (origin[i].b_is_center == true)
        {
          dest.x -= dest.w / 2;
          dest.y -= dest.h / 2;
        }
      Uint8 sprite_r = 255u;
      Uint8 sprite_g = 255u;
      Uint8 sprite_b = 255u;
      Uint8 sprite_alpha = 255u;
      if (sprite[i].b_uses_color == true)
        {
          if (alpha_opt != NULL && &alpha_opt[i] != NULL)
            {
              sprite_alpha = alpha_opt[i].value;
            }
          if (color_opt != NULL && &color_opt[i] != NULL)
            {
              sprite_r = color_opt[i].r;
              sprite_g = color_opt[i].g;
              sprite_b = color_opt[i].b;
            }
        }

      /* Enable a render target if needs be. */

      if (sprite[i].b_should_cache == true)
        {
          if (sprite[i].b_should_regenerate == false)
            {
              continue;
            }
          render_target_switch (core->rend, core->rts, sprite[i].cache_name);
          sprite[i].b_should_regenerate = false;
        }

      struct sprite_params params = { .opacity = sprite_alpha,
                                      .tint_r = sprite_r,
                                      .tint_g = sprite_g,
                                      .tint_b = sprite_b };
      switch (sprite[i].render_type)
        {
        case SPRITE_RENDER_TYPE_DEFAULT:
          {
            satlas_render_entry (core->atlas, sprite[i].name, &dest, &params);
            break;
          }
        case SPRITE_RENDER_TYPE_ANIMATED:
          {
            if (anim_player_opt != NULL && &anim_player_opt[i] != NULL)
              {
                satlas_render_entry_subsection (core->atlas, sprite[i].name,
                                                &anim_player_opt[i].subsection,
                                                &dest, &params);
              }
            break;
          }
        case SPRITE_RENDER_TYPE_TARGET:
          {
            struct render_target *rt
                = *dict_render_target_get (core->rts, sprite[i].name);
            SDL_RenderTexture (core->rend, rt->texture, NULL, &dest);
            break;
          }
        default:
          {
            break;
          }
        }

      if (sprite[i].b_should_cache == true)
        {
          render_target_switch (core->rend, core->rts, STRING_CTE (""));
        }
    }
}

static void
system_text_draw (ecs_iter_t *it)
{
  const core_s *core = ecs_singleton_get (it->world, core_s);

  text_c *text = ecs_field (it, text_c, 0);

  origin_c *origin = ecs_field (it, origin_c, 1);
  Sint8 alpha_id = 2;
  alpha_c *alpha_opt = NULL;
  if (ecs_field_is_set (it, alpha_id) == true)
    {
      alpha_opt = ecs_field (it, alpha_c, alpha_id);
    }
  Sint8 color_id = 3;
  color_c *color_opt = NULL;
  if (ecs_field_is_set (it, color_id) == true)
    {
      color_opt = ecs_field (it, color_c, color_id);
    }

  for (Sint32 i = 0; i < it->count; i++)
    {
      if (ecs_has_pair (it->world, it->entities[i],
                        ecs_lookup (it->world, "scene"), EcsAny)
              == true
          && ecs_has_pair (it->world, it->entities[i],
                           ecs_lookup (it->world, "scene"),
                           core->current_scene)
                 == false)
        {
          continue;
        }
      if (text[i].b_is_shown == false)
        {
          continue;
        }
      Uint8 text_r = 255u;
      Uint8 text_g = 255u;
      Uint8 text_b = 255u;
      Uint8 alpha = 255u;
      if (text[i].b_uses_color == true)
        {
          if (color_opt != NULL && &color_opt[i] != NULL)
            {
              text_r = color_opt[i].r;
              text_g = color_opt[i].g;
              text_b = color_opt[i].b;
            }
          if (alpha_opt != NULL && &alpha_opt[i] != NULL)
            {
              alpha = alpha_opt[i].value;
            }
        }
      SDL_FPoint dest = origin[i].world;
      if (origin[i].b_can_be_scaled == true)
        {
          dest.x *= core->scale;
          dest.y *= core->scale;
        }
      struct text_params params = { .size = text[i].font_size,
                                    .tint_r = text_r,
                                    .tint_g = text_g,
                                    .tint_b = text_b,
                                    .alpha = alpha,
                                    .face = "baby_jeepers",
                                    .location = dest,
                                    .align_h = text[i].align_h,
                                    .align_v = text[i].align_v };
      text_man_render_string (core->text_man, &params, text[i].content);
    }
}

/******************************/
/*********** Init *************/
/******************************/

static void
init_pluto_systems (ecs_world_t *ecs)
{
  {
    ecs_entity_t ent
        = ecs_entity (ecs, { .name = "system_origin_bind_relative",
                             .add = ecs_ids (ecs_dependson (
                                 ecs_lookup (ecs, "pre_render_phase"))) });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (origin_c) } } };
    ecs_system (ecs, { .entity = ent,
                       .query = query,
                       .callback = system_origin_bind_relative });
  }
  {
    ecs_entity_t ent
        = ecs_entity (ecs, { .name = "system_origin_calc_world",
                             .add = ecs_ids (ecs_dependson (
                                 ecs_lookup (ecs, "pre_render_phase"))) });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (origin_c) },
                                          { .id = ecs_id (origin_c),
                                            .src = { .id = EcsUp },
                                            .oper = EcsOptional } } };
    ecs_system (ecs, { .entity = ent,
                       .query = query,
                       .callback = system_origin_calc_world });
  }
  {
    ecs_entity_t ent
        = ecs_entity (ecs, { .name = "system_hover_toggle",
                             .add = ecs_ids (ecs_dependson (
                                 ecs_lookup (ecs, "pre_render_phase"))) });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (hover_c) },
                                          { .id = ecs_id (origin_c) },
                                          { .id = ecs_id (bounds_c) } } };
    ecs_system (
        ecs,
        { .entity = ent, .query = query, .callback = system_hover_toggle });
  }
  {
    ecs_entity_t ent
        = ecs_entity (ecs, { .name = "system_click_toggle",
                             .add = ecs_ids (ecs_dependson (
                                 ecs_lookup (ecs, "pre_render_phase"))) });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (click_c) },
                                          { .id = ecs_id (hover_c) } } };
    ecs_system (
        ecs,
        { .entity = ent, .query = query, .callback = system_click_toggle });
  }
  {
    ecs_entity_t ent
        = ecs_entity (ecs, { .name = "system_color_set",
                             .add = ecs_ids (ecs_dependson (
                                 ecs_lookup (ecs, "pre_render_phase"))) });
    ecs_query_desc_t query
        = { .terms = { { .id = ecs_id (color_c) },
                       { .id = ecs_id (hover_c), .oper = EcsOptional },
                       { .id = ecs_id (click_c), .oper = EcsOptional } } };
    ecs_system (
        ecs, { .entity = ent, .query = query, .callback = system_color_set });
  }
  {
    ecs_entity_t ent = ecs_entity (
        ecs,
        { .name = "system_box_draw",
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
  {
    ecs_entity_t ent = ecs_entity (
        ecs,
        { .name = "system_sprite_draw",
          .add = ecs_ids (ecs_dependson (ecs_lookup (ecs, "render_phase"))) });
    ecs_query_desc_t query
        = { .terms
            = { { .id = ecs_id (sprite_c) },
                { .id = ecs_id (bounds_c) },
                { .id = ecs_id (origin_c) },
                { .id = ecs_id (alpha_c), .oper = EcsOptional },
                { .id = ecs_id (color_c), .oper = EcsOptional },
                { .id = ecs_id (anim_player_c), .oper = EcsOptional } } };
    ecs_system (
        ecs,
        { .entity = ent, .query = query, .callback = system_sprite_draw });
  }
  {
    ecs_entity_t ent = ecs_entity (
        ecs,
        { .name = "system_text_draw",
          .add = ecs_ids (ecs_dependson (ecs_lookup (ecs, "render_phase"))) });
    ecs_query_desc_t query
        = { .terms = { { .id = ecs_id (text_c) },
                       { .id = ecs_id (origin_c) },
                       { .id = ecs_id (alpha_c), .oper = EcsOptional },
                       { .id = ecs_id (color_c), .oper = EcsOptional } } };
    ecs_system (
        ecs, { .entity = ent, .query = query, .callback = system_text_draw });
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

  ecs_type_hooks_t anim_player_hooks = { .ctor = anim_player };
  ecs_set_hooks_id (ecs, ecs_id (anim_player_c), &anim_player_hooks);

  ecs_type_hooks_t bounds_hooks = { .ctor = bounds };
  ecs_set_hooks_id (ecs, ecs_id (bounds_c), &bounds_hooks);

  ecs_type_hooks_t box_hooks = { .ctor = box };
  ecs_set_hooks_id (ecs, ecs_id (box_c), &box_hooks);

  ecs_type_hooks_t click_hooks = { .ctor = click };
  ecs_set_hooks_id (ecs, ecs_id (click_c), &click_hooks);

  ecs_type_hooks_t color_hooks = { .ctor = color };
  ecs_set_hooks_id (ecs, ecs_id (color_c), &color_hooks);

  ecs_type_hooks_t hover_hooks = { .ctor = hover };
  ecs_set_hooks_id (ecs, ecs_id (hover_c), &hover_hooks);

  ecs_type_hooks_t origin_hooks = { .ctor = origin };
  ecs_set_hooks_id (ecs, ecs_id (origin_c), &origin_hooks);

  ecs_type_hooks_t sprite_hooks = { .ctor = sprite };
  ecs_set_hooks_id (ecs, ecs_id (sprite_c), &sprite_hooks);

  ecs_type_hooks_t text_hooks = { .ctor = text };
  ecs_set_hooks_id (ecs, ecs_id (text_c), &text_hooks);
}

static core_s *
init_pluto_sdl (ecs_world_t *ecs, const SDL_Point window_size)
{
  core_s *core = ecs_singleton_ensure (ecs, core_s);
  SDL_SetHint ("SDL_WINDOWS_DPI_AWARENESS", "1");
  SDL_Init (SDL_INIT_VIDEO);
  TTF_Init ();
  core->win = SDL_CreateWindow ("Console", window_size.x, window_size.y,
                                SDL_WINDOW_RESIZABLE);
  core->rend = SDL_CreateRenderer (core->win, NULL);
  SDL_SetRenderVSync (core->rend, true);
  SDL_SetHint (SDL_HINT_GPU_DRIVER, "vulkan");
  SDL_SetHint (SDL_HINT_RENDER_GPU_DEBUG, "1");
  SDL_SetRenderDrawBlendMode (core->rend, SDL_BLENDMODE_BLEND);

  Sint32 compiled_v = SDL_VERSION;
  Sint32 linked_v = SDL_GetVersion ();
  log_debug (
      DEBUG_LOG_NONE,
      "Binary compiled with SDL %d.%d.%d. Binary linked against SDL %d.%d.%d.",
      SDL_VERSIONNUM_MAJOR (compiled_v), SDL_VERSIONNUM_MINOR (compiled_v),
      SDL_VERSIONNUM_MICRO (compiled_v), SDL_VERSIONNUM_MAJOR (linked_v),
      SDL_VERSIONNUM_MINOR (linked_v), SDL_VERSIONNUM_MICRO (linked_v));

  core->scale = 1.f;
  core->frame_data = SDL_calloc (1, sizeof (struct frame_data));

  satlas_init (core->rend, &core->atlas);
  satlas_dir_to_sheets (core->atlas, "dat/gfx/test", false,
                        STRING_CTE ("test"));

  text_man_create_font_book (&core->text_man, core->rend);

  struct input_man_callbacks callbacks
      = { .key_press_callback = handle_key_press,
          .key_release_callback = handle_key_release,
          .key_hold_callback = handle_key_hold,
          .mouse_press_callback = handle_mouse_press,
          .mouse_release_callback = handle_mouse_release,
          .mouse_hold_callback = handle_mouse_hold,
          .mouse_motion_callback = handle_mouse_motion };
  input_man_init (&core->input_man, &callbacks);

  return core;
}

core_s *
init_pluto (ecs_world_t *ecs, const SDL_Point window_size)
{
  ECS_COMPONENT_DEFINE (ecs, core_s);

  ECS_COMPONENT_DEFINE (ecs, alpha_c);
  ECS_COMPONENT_DEFINE (ecs, anim_player_c);
  ECS_COMPONENT_DEFINE (ecs, bounds_c);
  ECS_COMPONENT_DEFINE (ecs, box_c);
  ECS_COMPONENT_DEFINE (ecs, click_c);
  ECS_COMPONENT_DEFINE (ecs, color_c);
  ECS_COMPONENT_DEFINE (ecs, hover_c);
  ECS_COMPONENT_DEFINE (ecs, origin_c);
  ECS_COMPONENT_DEFINE (ecs, sprite_c);
  ECS_COMPONENT_DEFINE (ecs, text_c);
  core_s *core = init_pluto_sdl (ecs, window_size);
  init_pluto_hooks (ecs);
  init_pluto_phases (ecs);
  init_pluto_systems (ecs);
  return core;
}
