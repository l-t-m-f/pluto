#include "SDL3/SDL.h"
#include "flecs.h"

#include "pluto.h"

#include "animation.h"
#include "input_man.h"
#include "log.h"
#include "text_man.h"

Sint32 DEBUG_LOG = DEBUG_LOG_NONE;

void
handle_key_press (struct input_man *input_man, SDL_Scancode key, void *param)
{
}
void
handle_key_release (struct input_man *input_man, SDL_Scancode key, void *param)
{
}
void
handle_key_hold (struct input_man *input_man, SDL_Scancode key, void *param)
{
}
void
handle_mouse_press (struct input_man *input_man, SDL_FPoint pos, Uint8 button,
                    void *param)
{
  ecs_run (param, ecs_lookup (param, "system_margins_check_handles"), 0.f,
           NULL);
}
void
handle_mouse_release (struct input_man *input_man, SDL_FPoint pos,
                      Uint8 button, void *param)
{
  struct pluto_input_data *input_data = input_man->custom_data;
  input_data->b_is_resizing_widget = false;
  input_data->b_is_dragging_widget = false;
  ecs_run (param, ecs_lookup (param, "system_margins_check_handles"), 0.f,
           NULL);
}
void
handle_mouse_hold (struct input_man *input_man, SDL_FPoint pos, Uint8 button,
                   void *param)
{
}
void
handle_mouse_motion (struct input_man *input_man, SDL_FPoint pos,
                     SDL_FPoint rel, void *param)
{
  ecs_run (param, ecs_lookup (param, "system_drag_apply_delta"), 0.f, NULL);
  ecs_run (param, ecs_lookup (param, "system_resize_apply_delta"), 0.f, NULL);
}

int
main (int argc, char *argv[])
{
  ecs_world_t *world = ecs_init ();
  struct pluto_core_params params
      = { .init_flags = SDL_INIT_VIDEO,
          .window_name = "Console",
          .initial_window_size = (SDL_Point){ 640, 480 },
          .initial_layout_size = (SDL_Point){ 640, 480 },
          .window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY,
          .default_user_scaling = 1.f,
          .renderer_blend_mode = SDL_BLENDMODE_BLEND,
          .gpu_driver_hint = "vulkan",
          .b_is_DPI_aware = true,
          .b_should_debug_GPU = true,
          .input_data = { .b_is_resizing_widget = false,
                          .b_is_dragging_widget = false,
                          .b_is_moving_camera = false },
          .initial_constant_scroll_speed = 4.f,
          .initial_scroll_style = PLUTO_SCROLL_STYLE_CONSTANT,
          .b_should_initially_clamp_scroll_x = true,
          .b_should_initially_clamp_scroll_y = true,
          .initial_scroll_poll_frequency_ms = 2u,
          .initial_scroll_padding = {
              .top = 0,
              .left = 0,
              .bottom = 0,
              .right = 0,
          } };
  core_s *core = init_pluto (world, &params);

  satlas_dir_to_sheets (core->atlas, "dat/gfx/test", false,
                        STRING_CTE ("test"));
  render_target_add_to_pool (core->rts, STRING_CTE ("RT"),
                             (SDL_Point){ 320, 240 });

  /* Test box */
  {
    ecs_entity_t ent = ecs_entity (world, { .name = "box1" });
    bounds_c *bounds = ecs_ensure (world, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 70.f, 70.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (world, ent, box_c);
    box->b_uses_color = false;
    box->b_is_shown = true;
    box->b_is_filled = false;
    click_c *click = ecs_ensure (world, ent, click_c);
    click->toggled_r = 255u;
    click->toggled_g = 255u;
    click->toggled_b = 0u;
    ecs_add (world, ent, drag_c);
    color_c *color = ecs_ensure (world, ent, color_c);
    color->default_r = 0u;
    color->default_g = 0u;
    color->default_b = 0u;
    ecs_add (world, ent, hover_c);
    margins_c *margins = ecs_ensure (world, ent, margins_c);
    margins->value = (struct margins_ngrid){
      .top = 12.f, .left = 12.f, .bottom = 12.f, .right = 12.f
    };
    origin_c *origin = ecs_ensure (world, ent, origin_c);
    origin->relative = (SDL_FPoint){ 100.f, 200.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
    ecs_add (world, ent, resize_c);
    text_c *text = ecs_ensure (world, ent, text_c);
    string_init_set_str (text->content, "hello, world");
    text->font_size = 4u;
    text->align_h = TEXT_ALIGN_H_LEFT;
    text->align_v = TEXT_ALIGN_V_CENTER;
    text->b_is_shown = true;
    text->b_uses_color = true;
  }

  /* Child box test */
  {
    ecs_entity_t ent = ecs_entity (world, { .name = "box2" });
    alpha_c *alpha = ecs_ensure (world, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (world, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 15.f, 15.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (world, ent, box_c);
    box->b_uses_color = true;
    box->b_is_shown = true;
    box->b_is_filled = true;
    click_c *click = ecs_ensure (world, ent, click_c);
    click->toggled_r = 255u;
    click->toggled_g = 255u;
    click->toggled_b = 0u;
    color_c *color = ecs_ensure (world, ent, color_c);
    color->default_r = 255u;
    color->default_g = 0u;
    color->default_b = 0u;
    ecs_add (world, ent, drag_c);
    ecs_add (world, ent, hover_c);
    origin_c *origin = ecs_ensure (world, ent, origin_c);
    origin->relative = (SDL_FPoint){ 80.f, 80.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
    ecs_add_pair (world, ent, EcsChildOf, ecs_lookup (world, "box1"));
  }

  /* Mouse cursor test */
  {
    ecs_entity_t ent = ecs_entity (world, { .name = "mouse" });
    alpha_c *alpha = ecs_ensure (world, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (world, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 32.f, 32.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (world, ent, box_c);
    box->b_uses_color = true;
    box->b_is_shown = true;
    box->b_is_filled = false;
    color_c *color = ecs_ensure (world, ent, color_c);
    color->default_r = 255u;
    color->default_g = 0u;
    color->default_b = 0u;
    origin_c *origin = ecs_ensure (world, ent, origin_c);
    origin->relative_callback = get_mouse_position;
    origin->b_can_be_scaled = false;
    origin->b_is_center = false;
    sprite_c *sprite = ecs_ensure (world, ent, sprite_c);
    sprite->b_uses_color = false;
    string_init_set_str (sprite->name, "T_Cursor.png");
  }

  /* Animation test */
  {
    ecs_entity_t ent = ecs_entity (world, { .name = "animation" });
    anim_player_c *anim_player = ecs_ensure (world, ent, anim_player_c);
    struct anim_flipbook *flipbook
        = SDL_malloc (sizeof (struct anim_flipbook));
    flipbook->play_speed = 24u;
    flipbook->frame_size = (SDL_FPoint){ 64.f, 96.f };
    flipbook->frame_count = (SDL_Point){ 2, 1 };
    string_init_set_str (flipbook->name, "T_Animation.png");
    struct anim_pose *pose = SDL_malloc (sizeof (struct anim_pose));
    dict_sint32_anim_flipbook_init (pose->directions);
    dict_sint32_anim_flipbook_set_at (pose->directions, 0, flipbook);
    dict_string_anim_pose_init (anim_player->poses);
    dict_string_anim_pose_set_at (anim_player->poses, STRING_CTE ("idle"),
                                  pose);
    string_init_set_str (anim_player->control_pose, "idle");
    anim_player->control_direction = 0;
    alpha_c *alpha = ecs_ensure (world, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (world, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 64.f, 96.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (world, ent, box_c);
    box->b_uses_color = false;
    box->b_is_shown = true;
    box->b_is_filled = false;
    color_c *color = ecs_ensure (world, ent, color_c);
    color->default_r = 55u;
    color->default_g = 155u;
    color->default_b = 0u;
    origin_c *origin = ecs_ensure (world, ent, origin_c);
    origin->relative = (SDL_FPoint){ 40.f, 40.f };
    origin->b_can_be_scaled = false;
    origin->b_is_center = false;
    sprite_c *sprite = ecs_ensure (world, ent, sprite_c);
    sprite->b_uses_color = false;
  }

  /* Ngrid test */
  {
    ecs_entity_t ent = ecs_entity (world, { .name = "ngrid" });
    alpha_c *alpha = ecs_ensure (world, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (world, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 300.f, 240.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (world, ent, box_c);
    box->b_uses_color = false;
    box->b_is_shown = true;
    box->b_is_filled = false;
    color_c *color = ecs_ensure (world, ent, color_c);
    color->default_r = 55u;
    color->default_g = 155u;
    color->default_b = 0u;
    origin_c *origin = ecs_ensure (world, ent, origin_c);
    origin->relative = (SDL_FPoint){ 300.f, 40.f };
    origin->b_can_be_scaled = false;
    origin->b_is_center = false;
    margins_c *margins = ecs_ensure (world, ent, margins_c);
    margins->value = (struct margins_ngrid){
      .top = 70.f, .left = 70.f, .bottom = 70.f, .right = 70.f
    };
    ngrid_c *ngrid = ecs_ensure (world, ent, ngrid_c);
    string_init_set_str (ngrid->name, "T_Ngrid.png");
  }

  /* Child box test */
  {
    ecs_entity_t ent = ecs_entity (world, { .name = "box3" });
    alpha_c *alpha = ecs_ensure (world, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (world, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 115.f, 145.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (world, ent, box_c);
    box->b_uses_color = true;
    box->b_is_shown = true;
    box->b_is_filled = true;
    click_c *click = ecs_ensure (world, ent, click_c);
    click->toggled_r = 255u;
    click->toggled_g = 255u;
    click->toggled_b = 0u;
    color_c *color = ecs_ensure (world, ent, color_c);
    color->default_r = 255u;
    color->default_g = 0u;
    color->default_b = 0u;
    ecs_add (world, ent, drag_c);
    ecs_add (world, ent, hover_c);
    origin_c *origin = ecs_ensure (world, ent, origin_c);
    origin->relative = (SDL_FPoint){ 280.f, 80.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
    pattern_c *pattern = ecs_ensure (world, ent, pattern_c);
    string_init_set_str (pattern->name, "T_Pattern.png");
    ecs_add_pair (world, ent, EcsChildOf, ecs_lookup (world, "box1"));
  }

  /* Cached sprite test */
  {
    ecs_entity_t ent = ecs_entity (world, { .name = "cached_sprite" });
    bounds_c *bounds = ecs_ensure (world, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 64.f, 64.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (world, ent, box_c);
    box->b_is_shown = false;
    box->b_is_filled = false;
    cache_c *cache = ecs_ensure (world, ent, cache_c);
    string_init_set_str (cache->cache_name, "RT");
    cache->b_should_regenerate = true;
    origin_c *origin = ecs_ensure (world, ent, origin_c);
    origin->relative = (SDL_FPoint){ 0.f, 0.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
    sprite_c *sprite = ecs_ensure (world, ent, sprite_c);
    string_init_set_str (sprite->name, "T_Tile.png");
  }

  /* Render_target test */
  {
    ecs_entity_t ent = ecs_entity (world, { .name = "RT" });
    bounds_c *bounds = ecs_ensure (world, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 320.f, 240.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (world, ent, box_c);
    box->b_is_shown = true;
    box->b_is_filled = false;
    origin_c *origin = ecs_ensure (world, ent, origin_c);
    origin->relative = (SDL_FPoint){ 0.f, 0.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
    render_target_c *render_target = ecs_ensure (world, ent, render_target_c);
    string_init_set_str (render_target->name, "RT");
  }

  SDL_Event e;
  while (1)
    {
      while (SDL_PollEvent (&e) > 0)
        {
          if (e.type == SDL_EVENT_QUIT)
            {
              SDL_Quit ();
              exit (0);
            }
          if (e.type == SDL_EVENT_KEY_DOWN)
            {
              input_man_register_scancode (core->input_man, e.key.scancode,
                                           world);
            }
          if (e.type == SDL_EVENT_KEY_UP)
            {
              input_man_unregister_scancode (core->input_man, e.key.scancode,
                                             world);
            }
          if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
              input_man_try_handle_mouse_motion (core->input_man, e.motion,
                                                 world);
            }
          if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
              input_man_try_handle_mouse_down (core->input_man, e.button,
                                               world);
            }

          if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
              input_man_try_handle_mouse_up (core->input_man, e.button, world);
            }
        }
      input_man_bounce_keys (core->input_man, world);
      SDL_SetRenderDrawColor (core->rend, 0, 0, 67, 255);
      SDL_RenderClear (core->rend);
      ecs_progress (world, 0.f);
      SDL_RenderPresent (core->rend);
    }

  return 0;
}
