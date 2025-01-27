#include "pluto.h"

#include "SDL3_ttf/SDL_ttf.h"

#include "game_modules/input_man.h"
#include "game_modules/log.h"
#include "game_modules/profiling.h"
#include "game_modules/sprite_atlas.h"
#include "game_modules/text_man.h"

ECS_COMPONENT_DECLARE (core_s);

ECS_COMPONENT_DECLARE (array_c);
ECS_COMPONENT_DECLARE (alpha_c);
ECS_COMPONENT_DECLARE (anim_player_c);
ECS_COMPONENT_DECLARE (bounds_c);
ECS_COMPONENT_DECLARE (box_c);
ECS_COMPONENT_DECLARE (cache_c);
ECS_COMPONENT_DECLARE (click_c);
ECS_COMPONENT_DECLARE (color_c);
ECS_COMPONENT_DECLARE (drag_c);
ECS_COMPONENT_DECLARE (hover_c);
ECS_COMPONENT_DECLARE (index_c);
ECS_COMPONENT_DECLARE (mat2d_c);
ECS_COMPONENT_DECLARE (mat3d_c);
ECS_COMPONENT_DECLARE (margins_c);
ECS_COMPONENT_DECLARE (ngrid_c);
ECS_COMPONENT_DECLARE (origin_c);
ECS_COMPONENT_DECLARE (pattern_c);
ECS_COMPONENT_DECLARE (render_target_c);
ECS_COMPONENT_DECLARE (resize_c);
ECS_COMPONENT_DECLARE (sprite_c);
ECS_COMPONENT_DECLARE (text_c);

/******************************/
/*********** Hooks ************/
/******************************/

static void
array (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  array_c *array = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      arr_entity_init (array[i].content);
    }
}

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
cache (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  cache_c *cache = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      cache[i].b_should_regenerate = false;
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
drag (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  drag_c *drag = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      drag[i].toggled_r = 0u;
      drag[i].toggled_g = 255u;
      drag[i].toggled_b = 0u;
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
index (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  index_c *index = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      index[i].x = 0;
      index[i].y = 0;
      index[i].z = 0;
    }
}

static void
mat2d (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  mat2d_c *mat2d = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      mat2d_entity_init (mat2d[i].content);
    }
}

static void
mat3d (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  mat3d_c *mat3d = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      mat3d_entity_init (mat3d[i].content);
    }
}

void
margins (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  margins_c *margins = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      margins[i].value = (struct margins_ngrid){
        .top = 0.f, .left = 0.f, .bottom = 0.f, .right = 0.f
      };
      margins[i].edge_r = 155u;
      margins[i].edge_g = 155u;
      margins[i].edge_b = 155u;
      margins[i].edge_a = 255u;
      margins[i].corner_r = 255u;
      margins[i].corner_g = 0u;
      margins[i].corner_b = 95u;
      margins[i].corner_a = 255u;
      margins[i].current_handle = MARGIN_HANDLE_NONE;
    }
}

static void
ngrid (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  ngrid_c *ngrid = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      ngrid[i].b_is_shown = true;
      ngrid[i].b_uses_color = false;
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
pattern (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  pattern_c *pattern = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      pattern[i].b_is_shown = true;
      pattern[i].b_uses_color = false;
    }
}

void
render_target (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  render_target_c *render_target = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      render_target[i].b_is_shown = true;
      render_target[i].b_uses_color = false;
    }
}

void
resize (void *ptr, Sint32 count, const ecs_type_info_t *type_info)
{
  resize_c *resize = ptr;
  for (Sint32 i = 0; i < count; i++)
    {
      resize[i].b_state = false;
      SDL_memset (resize[i].from, true, sizeof (bool) * 9);
      resize[i].toggled_r = 255u;
      resize[i].toggled_g = 255u;
      resize[i].toggled_b = 255u;
      resize[i].toggled_a = 255u;
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
      sprite[i].offset = (SDL_FPoint){ 0.f, 0.f };
      sprite[i].b_overrides_bounds = false;
      sprite[i].over_size = (SDL_FPoint){ 0.f, 0.f };
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
      struct anim_pose *pose = *dict_string_anim_pose_get (
          anim_player[i].poses, anim_player[i].control_pose);

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
    }
}

static void
system_anim_player_assign_sprite_name (ecs_iter_t *it)
{
  anim_player_c *anim_player = ecs_field (it, anim_player_c, 0);
  sprite_c *sprite = ecs_field (it, sprite_c, 1);

  for (Sint32 i = 0; i < it->count; i++)
    {
      struct anim_pose *pose = *dict_string_anim_pose_get (
          anim_player[i].poses, anim_player[i].control_pose);

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
      struct anim_pose *pose = *dict_string_anim_pose_get (
          anim_player[i].poses, anim_player[i].control_pose);
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
  const core_s *core = ecs_singleton_get (it->world, core_s);
  struct custom_input_data *custom_data = core->input_man->custom_data;
  if (custom_data->b_is_dragging_widget == true
      || custom_data->b_is_resizing_widget == true)
    {
      return;
    }
  click_c *click = ecs_field (it, click_c, 0);

  hover_c *hover = ecs_field (it, hover_c, 1);

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

void
system_drag_apply_delta (ecs_iter_t *it)
{
  const core_s *core = ecs_singleton_get (it->world, core_s);
  struct custom_input_data *custom_data = core->input_man->custom_data;
  if (custom_data->b_is_resizing_widget == true)
    {
      return;
    }
  drag_c *drag = ecs_field (it, drag_c, 0);

  origin_c *origin = ecs_field (it, origin_c, 1);
  click_c *click = ecs_field (it, click_c, 2);

  for (Sint32 i = 0; i < it->count; i++)
    {

      ((struct custom_input_data *)core->input_man->custom_data)
          ->b_is_dragging_widget
          = false;
      SDL_FPoint delta = core->input_man->mouse.motion;
      if (origin[i].b_can_be_scaled == true)
        {
          delta.x *= core->scale;
          delta.y *= core->scale;
        }
      if (click[i].b_state == true)
        {
          drag[i].b_state = true;

          origin[i].relative.x += delta.x;
          origin[i].relative.y += delta.y;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_dragging_widget
              = true;
        }
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
      SDL_FPoint mouse_origin = core->input_man->mouse.position.window;
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

      if ((mouse_origin.x >= collider.x && mouse_origin.x <= collider.w)
          && (mouse_origin.y >= collider.y && mouse_origin.y <= collider.h))
        {
          hover[i].b_state = true;
        }
    }
}

void
system_margins_check_handles (ecs_iter_t *it)
{
  const core_s *core = ecs_singleton_get (it->world, core_s);
  const struct input_man *input_man = core->input_man;

  margins_c *margins = ecs_field (it, margins_c, 0);

  origin_c *origin = ecs_field (it, origin_c, 1);
  bounds_c *bounds = ecs_field (it, bounds_c, 2);

  for (Sint32 i = 0; i < it->count; i++)
    {
      SDL_FPoint pos = origin[i].world;
      SDL_FPoint size = bounds[i].size;
      struct margins_ngrid values = margins[i].value;

      if (origin[i].b_can_be_scaled == true)
        {
          pos.x *= core->scale;
          pos.y *= core->scale;
        }
      if (bounds[i].b_can_be_scaled == true)
        {
          size.x *= core->scale;
          size.y *= core->scale;
          values.top *= core->scale;
          values.left *= core->scale;
          values.bottom *= core->scale;
          values.right *= core->scale;
        }

      /* This part stores the 8 possible handles boundaries as four arrays.
       * The indices are labelled with comments. Each boundary array is
       * accessed during the inner loop and used to compute a temporary
       * rectangle corresponding to the handle's area to test against the
       * mouse position.
       */
      /* clang-format off */
      float left_boundaries[MARGIN_HANDLE_TYPE_MAX - 1] = {
        (pos.x + values.left),           /* Top */
        (pos.x),                          /* Top-left */
        (pos.x),                          /* Left */
        (pos.x),                          /* Bot-left */
        (pos.x + values.left),           /* Bot */
        (pos.x + size.x - values.right), /* Bot-right */
        (pos.x + size.x - values.right), /* Right */
        (pos.x + size.x - values.right), /* Top-right */
      };
      float top_boundaries[MARGIN_HANDLE_TYPE_MAX - 1] = {
        (pos.y),                           /* Top */
        (pos.y),                           /* Top-left */
        (pos.y + values.top),             /* Left */
        (pos.y + size.y - values.bottom), /* Bot-left */
        (pos.y + size.y - values.bottom), /* Bot */
        (pos.y + size.y - values.bottom), /* Bot-right */
        (pos.y + values.top),             /* Right */
        (pos.y),                           /* Top-right */
      };
      float right_boundaries[MARGIN_HANDLE_TYPE_MAX - 1] = {
        (pos.x + size.x - values.right),  /* Top */
        (pos.x + values.left),            /* Top-left */
        (pos.x + values.left),            /* Left */
        (pos.x + values.left),            /* Bot-left */
        (pos.x + size.x - values.right),  /* Bot */
        (pos.x + size.x),                  /* Bot-right */
        (pos.x + size.x),                  /* Right */
        (pos.x + size.x),                  /* Top-right */
      };
      float bot_boundaries[MARGIN_HANDLE_TYPE_MAX - 1] = {
        (pos.y + values.top),             /* Top */
        (pos.y + values.top),             /* Top-left */
        (pos.y + size.y - values.bottom), /* Left */
        (pos.y + size.y),                  /* Bot-left */
        (pos.y + size.y),                  /* Bot */
        (pos.y + size.y),                  /* Bot-right */
        (pos.y + size.y - values.bottom), /* Right */
        (pos.y + values.top),             /* Top-right */
      };
      /* clang-format on */

      bool match_found = false;

      for (Sint32 j = MARGIN_HANDLE_TOP_EDGE; j < MARGIN_HANDLE_TYPE_MAX; j++)
        {

          if (input_man->mouse.position.window.x > left_boundaries[j - 1]
              && input_man->mouse.position.window.x < right_boundaries[j - 1]
              && input_man->mouse.position.window.y > top_boundaries[j - 1]
              && input_man->mouse.position.window.y < bot_boundaries[j - 1])
            {
              margins[i].current_handle = j;
              match_found = true;
              break;
            }
        }
      if (match_found == false)
        {
          margins[i].current_handle = MARGIN_HANDLE_NONE;
        }
    }
}

void
system_margins_draw (ecs_iter_t *it)
{
  const core_s *core = ecs_singleton_get (it->world, core_s);

  margins_c *margins = ecs_field (it, margins_c, 0);

  origin_c *origin = ecs_field (it, origin_c, 1);
  bounds_c *bounds = ecs_field (it, bounds_c, 2);

  for (Sint32 i = 0; i < it->count; i++)
    {

      SDL_FPoint pos = origin[i].world;
      SDL_FPoint size = bounds[i].size;
      struct margins_ngrid margins_values
          = { margins[i].value.top, margins[i].value.left,
              margins[i].value.bottom, margins[i].value.right };

      if (margins_values.top == 0.f && margins_values.left == 0.f
          && margins_values.bottom == 0.f && margins_values.right == 0.f)
        {
          continue;
        }
      if (origin[i].b_can_be_scaled == true)
        {
          pos.x *= core->scale;
          pos.y *= core->scale;
        }
      if (bounds[i].b_can_be_scaled == true)
        {
          size.x *= core->scale;
          size.y *= core->scale;
          margins_values.top *= core->scale;
          margins_values.left *= core->scale;
          margins_values.bottom *= core->scale;
          margins_values.right *= core->scale;
        }

      const float offset = 0.f;

      /* Corners are specified from top-left counter clock-wise. */
      SDL_FRect top_left_corner
          = (SDL_FRect){ pos.x, pos.y, margins_values.left,
                         margins_values.top };

      SDL_FRect top_right_corner
          = (SDL_FRect){ pos.x + size.x - margins_values.right, pos.y,
                         margins_values.right, margins_values.top };

      SDL_FRect bottom_left_corner
          = (SDL_FRect){ pos.x, pos.y + size.y - margins_values.bottom,
                         margins_values.left, margins_values.bottom };

      SDL_FRect bottom_right_corner
          = (SDL_FRect){ pos.x + size.x - margins_values.right,
                         pos.y + size.y - margins_values.bottom,
                         margins_values.right, margins_values.bottom };

      /* Edges are specified in a WASD configuration. */
      SDL_FRect top_edge_rect
          = (SDL_FRect){ pos.x + margins_values.left, pos.y,
                         size.x - (margins_values.right * 2),
                         margins_values.top };

      SDL_FRect left_edge_rect
          = (SDL_FRect){ pos.x, pos.y + margins_values.top,
                         margins_values.left,
                         size.y - margins_values.bottom - margins_values.top };

      SDL_FRect bottom_edge_rect = (SDL_FRect){
        pos.x + margins_values.left, pos.y + size.y - margins_values.bottom,
        size.x - (margins_values.right * 2), margins_values.bottom
      };

      SDL_FRect right_edge_rect
          = (SDL_FRect){ pos.x + size.x - margins_values.right,
                         pos.y + margins_values.top, margins_values.right,
                         size.y - margins_values.bottom - margins_values.top };

      top_left_corner.x += offset;
      top_left_corner.y += offset;
      top_left_corner.w -= offset;
      top_left_corner.h -= offset;

      top_right_corner.x += offset;
      top_right_corner.y += offset;
      top_right_corner.w -= offset * 2;
      top_right_corner.h -= offset;

      bottom_left_corner.x += offset;
      bottom_left_corner.y += offset;
      bottom_left_corner.w -= offset;
      bottom_left_corner.h -= offset * 2;

      bottom_right_corner.x += offset;
      bottom_right_corner.y += offset;
      bottom_right_corner.w -= offset * 2;
      bottom_right_corner.h -= offset * 2;

      top_edge_rect.x += offset;
      top_edge_rect.y += offset;
      top_edge_rect.w -= offset;
      top_edge_rect.h -= offset;

      left_edge_rect.x += offset;
      left_edge_rect.y += offset;
      left_edge_rect.w -= offset;
      left_edge_rect.h -= offset;

      bottom_edge_rect.x += offset;
      bottom_edge_rect.y += offset;
      bottom_edge_rect.w -= offset;
      bottom_edge_rect.h -= offset * 2;

      right_edge_rect.x += offset;
      right_edge_rect.y += offset;
      right_edge_rect.w -= offset * 2;
      right_edge_rect.h -= offset;

      SDL_SetRenderDrawColor (core->rend, margins[i].corner_r,
                              margins[i].corner_g, margins[i].corner_b,
                              margins[i].corner_a);

      SDL_RenderRect (core->rend, &top_left_corner);
      SDL_RenderRect (core->rend, &top_right_corner);
      SDL_RenderRect (core->rend, &bottom_left_corner);
      SDL_RenderRect (core->rend, &bottom_right_corner);

      SDL_SetRenderDrawColor (core->rend, margins[i].edge_r, margins[i].edge_g,
                              margins[i].edge_b, margins[i].edge_a);

      SDL_RenderRect (core->rend, &top_edge_rect);
      SDL_RenderRect (core->rend, &bottom_edge_rect);
      SDL_RenderRect (core->rend, &left_edge_rect);
      SDL_RenderRect (core->rend, &right_edge_rect);
    }
}

static void
system_ngrid_draw (ecs_iter_t *it)
{
  ngrid_c *ngrid = ecs_field (it, ngrid_c, 0);

  bounds_c *bounds = ecs_field (it, bounds_c, 1);
  origin_c *origin = ecs_field (it, origin_c, 2);
  margins_c *margins = ecs_field (it, margins_c, 3);
  Sint8 alpha_id = 4;
  alpha_c *alpha_opt = NULL;
  if (ecs_field_is_set (it, alpha_id) == true)
    {
      alpha_opt = ecs_field (it, alpha_c, alpha_id);
    }
  Sint8 color_id = 5;
  color_c *color_opt = NULL;
  if (ecs_field_is_set (it, color_id) == true)
    {
      color_opt = ecs_field (it, color_c, color_id);
    }
  Sint8 anim_player_id = 6;
  anim_player_c *anim_player_opt = NULL;
  if (ecs_field_is_set (it, anim_player_id) == true)
    {
      anim_player_opt = ecs_field (it, anim_player_c, anim_player_id);
    }
  Sint8 cache_id = 7;
  cache_c *cache_opt = NULL;
  if (ecs_field_is_set (it, cache_id) == true)
    {
      cache_opt = ecs_field (it, cache_c, cache_id);
    }

  core_s *core = ecs_get_mut (it->world, ecs_id (core_s), core_s);

  for (Sint32 i = 0; i < it->count; i++)
    {
      if (ngrid[i].b_is_shown == false)
        {
          continue;
        }
      SDL_FRect dest = { .x = origin[i].world.x, .y = origin[i].world.y };

      if (origin[i].b_can_be_scaled == true)
        {
          dest.x *= core->scale;
          dest.y *= core->scale;
        }

      dest.w = bounds[i].size.x;
      dest.h = bounds[i].size.y;

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
      Uint8 ngrid_r = 255u;
      Uint8 ngrid_g = 255u;
      Uint8 ngrid_b = 255u;
      Uint8 ngrid_a = 255u;
      if (ngrid[i].b_uses_color == true)
        {
          if (alpha_opt != NULL && &alpha_opt[i] != NULL)
            {
              ngrid_a = alpha_opt[i].value;
            }
          if (color_opt != NULL && &color_opt[i] != NULL)
            {
              ngrid_r = color_opt[i].r;
              ngrid_g = color_opt[i].g;
              ngrid_b = color_opt[i].b;
            }
        }

      /* Enable a render target if needs be. */
      if (cache_opt != NULL && &cache_opt[i] != NULL)
        {
          if (cache_opt[i].b_should_regenerate == false)
            {
              continue;
            }
          render_target_switch (core->rend, core->rts,
                                cache_opt[i].cache_name);
          cache_opt[i].b_should_regenerate = false;
        }

      struct satlas_render_color_params params = { .opacity = ngrid_a,
                                                   .tint_r = ngrid_r,
                                                   .tint_g = ngrid_g,
                                                   .tint_b = ngrid_b };

      satlas_render_entry_ngrid (core->atlas, ngrid[i].name, &margins[i].value,
                                 core->scale, &dest, &params);

      if (cache_opt != NULL && &cache_opt[i] != NULL)
        {
          render_target_switch (core->rend, core->rts, STRING_CTE (""));
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

void
system_resize_apply_delta (ecs_iter_t *it)
{
  const core_s *core = ecs_singleton_get (it->world, core_s);
  struct custom_input_data *custom_data = core->input_man->custom_data;
  if (custom_data->b_is_dragging_widget == true)
    {
      return;
    }
  resize_c *resize = ecs_field (it, resize_c, 0);

  margins_c *margins = ecs_field (it, margins_c, 1);
  origin_c *origin = ecs_field (it, origin_c, 2);
  bounds_c *bounds = ecs_field (it, bounds_c, 3);
  click_c *click = ecs_field (it, click_c, 4);

  for (Sint32 i = 0; i < it->count; i++)
    {
      ((struct custom_input_data *)core->input_man->custom_data)
          ->b_is_resizing_widget
          = false;
      resize[i].b_state = false;
      if (click[i].b_state == false)
        {
          continue;
        }

      SDL_FPoint delta = core->input_man->mouse.motion;

      if (bounds[i].b_can_be_scaled == true)
        {
          delta.x *= core->scale;
          delta.y *= core->scale;
        }

      if (resize[i].from[MARGIN_HANDLE_BOT_RIGHT_CORNER] == true
          && margins[i].current_handle == MARGIN_HANDLE_TOP_RIGHT_CORNER)
        {
          bounds[i].size.x += delta.x;
          origin[i].relative.y += delta.y;
          bounds[i].size.y -= delta.y;
          resize[i].b_state = true;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_resizing_widget
              = true;
        }
      else if (resize[i].from[MARGIN_HANDLE_TOP_EDGE] == true
               && margins[i].current_handle == MARGIN_HANDLE_TOP_EDGE)
        {
          origin[i].relative.y += delta.y;
          bounds[i].size.y -= delta.y;
          resize[i].b_state = true;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_resizing_widget
              = true;
        }
      else if (resize[i].from[MARGIN_HANDLE_TOP_LEFT_CORNER] == true
               && margins[i].current_handle == MARGIN_HANDLE_TOP_LEFT_CORNER)
        {
          origin[i].relative.x += delta.x;
          bounds[i].size.x -= delta.x;
          origin[i].relative.y += delta.y;
          bounds[i].size.y -= delta.y;
          resize[i].b_state = true;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_resizing_widget
              = true;
        }
      else if (resize[i].from[MARGIN_HANDLE_LEFT_EDGE] == true
               && margins[i].current_handle == MARGIN_HANDLE_LEFT_EDGE)
        {
          origin[i].relative.x += delta.x;
          bounds[i].size.x -= delta.x;
          resize[i].b_state = true;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_resizing_widget
              = true;
        }
      else if (resize[i].from[MARGIN_HANDLE_BOT_LEFT_CORNER] == true
               && margins[i].current_handle == MARGIN_HANDLE_BOT_LEFT_CORNER)
        {
          origin[i].relative.x += delta.x;
          bounds[i].size.x -= delta.x;
          bounds[i].size.y += delta.y;
          resize[i].b_state = true;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_resizing_widget
              = true;
        }
      else if (resize[i].from[MARGIN_HANDLE_BOT_EDGE] == true
               && margins[i].current_handle == MARGIN_HANDLE_BOT_EDGE)
        {
          bounds[i].size.y += delta.y;
          resize[i].b_state = true;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_resizing_widget
              = true;
        }
      else if (resize[i].from[MARGIN_HANDLE_BOT_RIGHT_CORNER] == true
               && margins[i].current_handle == MARGIN_HANDLE_BOT_RIGHT_CORNER)
        {
          bounds[i].size.x += delta.x;
          bounds[i].size.y += delta.y;
          resize[i].b_state = true;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_resizing_widget
              = true;
        }
      else if (resize[i].from[MARGIN_HANDLE_RIGHT_EDGE] == true
               && margins[i].current_handle == MARGIN_HANDLE_RIGHT_EDGE)
        {
          bounds[i].size.x += delta.x;
          resize[i].b_state = true;

          ((struct custom_input_data *)core->input_man->custom_data)
              ->b_is_resizing_widget
              = true;
        }
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
system_pattern_draw (ecs_iter_t *it)
{
  pattern_c *pattern = ecs_field (it, pattern_c, 0);

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
  Sint8 cache_id = 6;
  cache_c *cache_opt = NULL;
  if (ecs_field_is_set (it, cache_id) == true)
    {
      cache_opt = ecs_field (it, cache_c, cache_id);
    }

  core_s *core = ecs_get_mut (it->world, ecs_id (core_s), core_s);

  for (Sint32 i = 0; i < it->count; i++)
    {
      if (pattern[i].b_is_shown == false)
        {
          continue;
        }
      SDL_FRect dest = { .x = origin[i].world.x, .y = origin[i].world.y };

      if (origin[i].b_can_be_scaled == true)
        {
          dest.x *= core->scale;
          dest.y *= core->scale;
        }

      dest.w = bounds[i].size.x;
      dest.h = bounds[i].size.y;

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
      Uint8 pattern_r = 255u;
      Uint8 pattern_g = 255u;
      Uint8 pattern_b = 255u;
      Uint8 pattern_a = 255u;
      if (pattern[i].b_uses_color == true)
        {
          if (alpha_opt != NULL && &alpha_opt[i] != NULL)
            {
              pattern_a = alpha_opt[i].value;
            }
          if (color_opt != NULL && &color_opt[i] != NULL)
            {
              pattern_r = color_opt[i].r;
              pattern_g = color_opt[i].g;
              pattern_b = color_opt[i].b;
            }
        }

      /* Enable a render target if needs be. */

      if (cache_opt != NULL && &cache_opt[i] != NULL)
        {
          if (cache_opt[i].b_should_regenerate == false)
            {
              continue;
            }
          render_target_switch (core->rend, core->rts,
                                cache_opt[i].cache_name);
          cache_opt[i].b_should_regenerate = false;
        }

      struct satlas_render_color_params params = { .opacity = pattern_a,
                                                   .tint_r = pattern_r,
                                                   .tint_g = pattern_g,
                                                   .tint_b = pattern_b };

      satlas_render_entry_tiled (core->atlas, pattern[i].name, &dest,
                                 core->scale, &params);

      if (cache_opt != NULL && &cache_opt[i] != NULL)
        {
          render_target_switch (core->rend, core->rts, STRING_CTE (""));
        }
    }
}

static void
system_render_target_draw (ecs_iter_t *it)
{
  render_target_c *render_target = ecs_field (it, render_target_c, 0);

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
  Sint8 cache_id = 5;
  cache_c *cache_opt = NULL;
  if (ecs_field_is_set (it, cache_id) == true)
    {
      cache_opt = ecs_field (it, cache_c, cache_id);
    }

  core_s *core = ecs_get_mut (it->world, ecs_id (core_s), core_s);

  for (Sint32 i = 0; i < it->count; i++)
    {
      if (render_target[i].b_is_shown == false)
        {
          continue;
        }
      SDL_FRect dest = { .x = origin[i].world.x, .y = origin[i].world.y };

      if (origin[i].b_can_be_scaled == true)
        {
          dest.x *= core->scale;
          dest.y *= core->scale;
        }

      dest.w = bounds[i].size.x;
      dest.h = bounds[i].size.y;

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
      Uint8 rt_r = 255u;
      Uint8 rt_g = 255u;
      Uint8 rt_b = 255u;
      Uint8 rt_a = 255u;
      if (render_target[i].b_uses_color == true)
        {
          if (alpha_opt != NULL && &alpha_opt[i] != NULL)
            {
              rt_a = alpha_opt[i].value;
            }
          if (color_opt != NULL && &color_opt[i] != NULL)
            {
              rt_r = color_opt[i].r;
              rt_g = color_opt[i].g;
              rt_b = color_opt[i].b;
            }
        }

      /* Enable a render target if needs be. */

      if (cache_opt != NULL && &cache_opt[i] != NULL)
        {
          if (cache_opt[i].b_should_regenerate == false)
            {
              continue;
            }
          render_target_switch (core->rend, core->rts,
                                cache_opt[i].cache_name);
          cache_opt[i].b_should_regenerate = false;
        }

      struct satlas_render_color_params params = {
        .opacity = rt_a, .tint_r = rt_r, .tint_g = rt_g, .tint_b = rt_b
      };

      const struct render_target *rt
          = *dict_render_target_get (core->rts, render_target[i].name);
      SDL_RenderTexture (core->rend, rt->texture, NULL, &dest);

      if (cache_opt != NULL && &cache_opt[i] != NULL)
        {
          render_target_switch (core->rend, core->rts, STRING_CTE (""));
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
  Sint8 cache_id = 6;
  cache_c *cache_opt = NULL;
  if (ecs_field_is_set (it, cache_id) == true)
    {
      cache_opt = ecs_field (it, cache_c, cache_id);
    }

  core_s *core = ecs_get_mut (it->world, ecs_id (core_s), core_s);

  for (Sint32 i = 0; i < it->count; i++)
    {
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

      if (cache_opt != NULL && &cache_opt[i] != NULL)
        {
          if (cache_opt[i].b_should_regenerate == false)
            {
              continue;
            }
          render_target_switch (core->rend, core->rts,
                                cache_opt[i].cache_name);
          cache_opt[i].b_should_regenerate = false;
        }

      struct satlas_render_color_params params = { .opacity = sprite_alpha,
                                                   .tint_r = sprite_r,
                                                   .tint_g = sprite_g,
                                                   .tint_b = sprite_b };

      if (anim_player_opt != NULL && &anim_player_opt[i] != NULL)
        {
          satlas_render_entry_subsection (core->atlas, sprite[i].name,
                                          &anim_player_opt[i].subsection,
                                          &dest, &params);
        }
      else
        {
          satlas_render_entry (core->atlas, sprite[i].name, &dest, &params);
        }

      if (cache_opt != NULL && &cache_opt[i] != NULL)
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
    ecs_entity_t ent = ecs_entity (ecs, {
                                            .name = "system_drag_apply_delta",
                                        });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (drag_c) },
                                          { .id = ecs_id (origin_c) },
                                          { .id = ecs_id (click_c) } } };
    ecs_system (ecs, { .entity = ent,
                       .query = query,
                       .callback = system_drag_apply_delta });
  }
  {
    ecs_entity_t ent
        = ecs_entity (ecs, {
                               .name = "system_margins_check_handles",
                           });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (margins_c) },
                                          { .id = ecs_id (origin_c) },
                                          { .id = ecs_id (bounds_c) } } };
    ecs_system (ecs, { .entity = ent,
                       .query = query,
                       .callback = system_margins_check_handles });
  }
  {
    ecs_entity_t ent
        = ecs_entity (ecs, {
                               .name = "system_resize_apply_delta",
                           });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (resize_c) },
                                          { .id = ecs_id (margins_c) },
                                          { .id = ecs_id (origin_c) },
                                          { .id = ecs_id (bounds_c) },
                                          { .id = ecs_id (click_c) } } };
    ecs_system (ecs, { .entity = ent,
                       .query = query,
                       .callback = system_resize_apply_delta });
  }

  /******************************************
   ******************************************/

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
        = ecs_entity (ecs, { .name = "system_anim_player_advance",
                             .add = ecs_ids (ecs_dependson (EcsOnUpdate)) });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (anim_player_c) } } };
    ecs_system (ecs, { .entity = ent,
                       .query = query,
                       .callback = system_anim_player_advance });
  }
  {
    ecs_entity_t ent
        = ecs_entity (ecs, { .name = "system_anim_player_assign_sprite_name",
                             .add = ecs_ids (ecs_dependson (EcsOnUpdate)) });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (anim_player_c) },
                                          { .id = ecs_id (sprite_c) } } };
    ecs_system (ecs, { .entity = ent,
                       .query = query,
                       .callback = system_anim_player_assign_sprite_name });
  }
  {
    ecs_entity_t ent
        = ecs_entity (ecs, { .name = "system_anim_player_set_subsection",
                             .add = ecs_ids (ecs_dependson (EcsOnUpdate)) });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (anim_player_c) } } };
    ecs_system (ecs, { .entity = ent,
                       .query = query,
                       .callback = system_anim_player_set_subsection });
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
        { .name = "system_margins_draw",
          .add = ecs_ids (ecs_dependson (ecs_lookup (ecs, "render_phase"))) });
    ecs_query_desc_t query = { .terms = { { .id = ecs_id (margins_c) },
                                          { .id = ecs_id (origin_c) },
                                          { .id = ecs_id (bounds_c) } } };
    ecs_system (
        ecs,
        { .entity = ent, .query = query, .callback = system_margins_draw });
  }
  {
    ecs_entity_t ent = ecs_entity (
        ecs,
        { .name = "system_ngrid_draw",
          .add = ecs_ids (ecs_dependson (ecs_lookup (ecs, "render_phase"))) });
    ecs_query_desc_t query
        = { .terms = { { .id = ecs_id (ngrid_c) },
                       { .id = ecs_id (bounds_c) },
                       { .id = ecs_id (origin_c) },
                       { .id = ecs_id (margins_c) },
                       { .id = ecs_id (alpha_c), .oper = EcsOptional },
                       { .id = ecs_id (color_c), .oper = EcsOptional },
                       { .id = ecs_id (anim_player_c), .oper = EcsOptional },
                       { .id = ecs_id (cache_c), .oper = EcsOptional } } };
    ecs_system (
        ecs, { .entity = ent, .query = query, .callback = system_ngrid_draw });
  }
  {
    ecs_entity_t ent = ecs_entity (
        ecs,
        { .name = "system_pattern_draw",
          .add = ecs_ids (ecs_dependson (ecs_lookup (ecs, "render_phase"))) });
    ecs_query_desc_t query
        = { .terms = { { .id = ecs_id (pattern_c) },
                       { .id = ecs_id (bounds_c) },
                       { .id = ecs_id (origin_c) },
                       { .id = ecs_id (alpha_c), .oper = EcsOptional },
                       { .id = ecs_id (color_c), .oper = EcsOptional },
                       { .id = ecs_id (anim_player_c), .oper = EcsOptional },
                       { .id = ecs_id (cache_c), .oper = EcsOptional } } };
    ecs_system (
        ecs,
        { .entity = ent, .query = query, .callback = system_pattern_draw });
  }
  {
    ecs_entity_t ent = ecs_entity (
        ecs,
        { .name = "system_sprite_draw",
          .add = ecs_ids (ecs_dependson (ecs_lookup (ecs, "render_phase"))) });
    ecs_query_desc_t query
        = { .terms = { { .id = ecs_id (sprite_c) },
                       { .id = ecs_id (bounds_c) },
                       { .id = ecs_id (origin_c) },
                       { .id = ecs_id (alpha_c), .oper = EcsOptional },
                       { .id = ecs_id (color_c), .oper = EcsOptional },
                       { .id = ecs_id (anim_player_c), .oper = EcsOptional },
                       { .id = ecs_id (cache_c), .oper = EcsOptional } } };
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
  {
    ecs_entity_t ent = ecs_entity (
        ecs,
        { .name = "system_render_target_draw",
          .add = ecs_ids (ecs_dependson (ecs_lookup (ecs, "render_phase"))) });
    ecs_query_desc_t query
        = { .terms = { { .id = ecs_id (render_target_c) },
                       { .id = ecs_id (bounds_c) },
                       { .id = ecs_id (origin_c) },
                       { .id = ecs_id (alpha_c), .oper = EcsOptional },
                       { .id = ecs_id (color_c), .oper = EcsOptional },
                       { .id = ecs_id (cache_c), .oper = EcsOptional } } };
    ecs_system (
        ecs,
        { .entity = ent, .query = query, .callback = system_render_target_draw });
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
  ecs_type_hooks_t array_hooks = { .ctor = array };
  ecs_set_hooks_id (ecs, ecs_id (array_c), &array_hooks);

  ecs_type_hooks_t alpha_hooks = { .ctor = alpha };
  ecs_set_hooks_id (ecs, ecs_id (alpha_c), &alpha_hooks);

  ecs_type_hooks_t anim_player_hooks = { .ctor = anim_player };
  ecs_set_hooks_id (ecs, ecs_id (anim_player_c), &anim_player_hooks);

  ecs_type_hooks_t bounds_hooks = { .ctor = bounds };
  ecs_set_hooks_id (ecs, ecs_id (bounds_c), &bounds_hooks);

  ecs_type_hooks_t box_hooks = { .ctor = box };
  ecs_set_hooks_id (ecs, ecs_id (box_c), &box_hooks);

  ecs_type_hooks_t cache_hooks = { .ctor = cache };
  ecs_set_hooks_id (ecs, ecs_id (cache_c), &cache_hooks);

  ecs_type_hooks_t click_hooks = { .ctor = click };
  ecs_set_hooks_id (ecs, ecs_id (click_c), &click_hooks);

  ecs_type_hooks_t color_hooks = { .ctor = color };
  ecs_set_hooks_id (ecs, ecs_id (color_c), &color_hooks);

  ecs_type_hooks_t drag_hooks = { .ctor = drag };
  ecs_set_hooks_id (ecs, ecs_id (drag_c), &drag_hooks);

  ecs_type_hooks_t hover_hooks = { .ctor = hover };
  ecs_set_hooks_id (ecs, ecs_id (hover_c), &hover_hooks);

  ecs_type_hooks_t index_hooks = { .ctor = index };
  ecs_set_hooks_id (ecs, ecs_id (index_c), &index_hooks);

  ecs_type_hooks_t mat2d_hooks = { .ctor = mat2d };
  ecs_set_hooks_id (ecs, ecs_id (mat2d_c), &mat2d_hooks);

  ecs_type_hooks_t mat3d_hooks = { .ctor = mat3d };
  ecs_set_hooks_id (ecs, ecs_id (mat3d_c), &mat3d_hooks);

  ecs_type_hooks_t margins_hooks = { .ctor = margins };
  ecs_set_hooks_id (ecs, ecs_id (margins_c), &margins_hooks);

  ecs_type_hooks_t ngrid_hooks = { .ctor = ngrid };
  ecs_set_hooks_id (ecs, ecs_id (ngrid_c), &ngrid_hooks);

  ecs_type_hooks_t origin_hooks = { .ctor = origin };
  ecs_set_hooks_id (ecs, ecs_id (origin_c), &origin_hooks);

  ecs_type_hooks_t pattern_hooks = { .ctor = pattern };
  ecs_set_hooks_id (ecs, ecs_id (pattern_c), &pattern_hooks);

  ecs_type_hooks_t render_target_hooks = { .ctor = render_target };
  ecs_set_hooks_id (ecs, ecs_id (render_target_c), &render_target_hooks);

  ecs_type_hooks_t resize_hooks = { .ctor = resize };
  ecs_set_hooks_id (ecs, ecs_id (resize_c), &resize_hooks);

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
  struct custom_input_data *custom_data
      = SDL_malloc (sizeof (struct custom_input_data));
  custom_data->b_is_resizing_widget = false;
  custom_data->b_is_dragging_widget = false;
  input_man_init (&core->input_man, &callbacks, custom_data);

  return core;
}

core_s *
init_pluto (ecs_world_t *ecs, const SDL_Point window_size)
{
  ECS_COMPONENT_DEFINE (ecs, core_s);

  ECS_COMPONENT_DEFINE (ecs, array_c);
  ECS_COMPONENT_DEFINE (ecs, alpha_c);
  ECS_COMPONENT_DEFINE (ecs, anim_player_c);
  ECS_COMPONENT_DEFINE (ecs, bounds_c);
  ECS_COMPONENT_DEFINE (ecs, box_c);
  ECS_COMPONENT_DEFINE (ecs, cache_c);
  ECS_COMPONENT_DEFINE (ecs, click_c);
  ECS_COMPONENT_DEFINE (ecs, color_c);
  ECS_COMPONENT_DEFINE (ecs, drag_c);
  ECS_COMPONENT_DEFINE (ecs, hover_c);
  ECS_COMPONENT_DEFINE (ecs, index_c);
  ECS_COMPONENT_DEFINE (ecs, mat2d_c);
  ECS_COMPONENT_DEFINE (ecs, mat3d_c);
  ECS_COMPONENT_DEFINE (ecs, margins_c);
  ECS_COMPONENT_DEFINE (ecs, ngrid_c);
  ECS_COMPONENT_DEFINE (ecs, origin_c);
  ECS_COMPONENT_DEFINE (ecs, pattern_c);
  ECS_COMPONENT_DEFINE (ecs, render_target_c);
  ECS_COMPONENT_DEFINE (ecs, resize_c);
  ECS_COMPONENT_DEFINE (ecs, sprite_c);
  ECS_COMPONENT_DEFINE (ecs, text_c);
  core_s *core = init_pluto_sdl (ecs, window_size);
  init_pluto_hooks (ecs);
  init_pluto_phases (ecs);
  init_pluto_systems (ecs);
  return core;
}
