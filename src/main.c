#include "SDL3/SDL.h"
#include "flecs.h"

#include "pluto.h"

#include "game_modules/animation.h"
#include "game_modules/input_man.h"
#include "game_modules/log.h"
#include "game_modules/text_man.h"

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
  struct custom_input_data *custom_data = input_man->custom_data;
  custom_data->b_is_resizing_widget = false;
  custom_data->b_is_dragging_widget = false;
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
  ecs_world_t *ecs = ecs_init ();
  core_s *core = init_pluto (ecs, (SDL_Point){ 640, 480 });

  ECS_COMPONENT (ecs, anim_player_c);
  ECS_COMPONENT (ecs, alpha_c);
  ECS_COMPONENT (ecs, bounds_c);
  ECS_COMPONENT (ecs, box_c);
  ECS_COMPONENT (ecs, click_c);
  ECS_COMPONENT (ecs, color_c);
  ECS_COMPONENT (ecs, drag_c);
  ECS_COMPONENT (ecs, hover_c);
  ECS_COMPONENT (ecs, margins_c);
  ECS_COMPONENT (ecs, ngrid_c);
  ECS_COMPONENT (ecs, origin_c);
  ECS_COMPONENT (ecs, resize_c);
  ECS_COMPONENT (ecs, sprite_c);
  ECS_COMPONENT (ecs, text_c);

  /* Test box */
  {
    ecs_entity_t ent = ecs_entity (ecs, { .name = "box1" });
    bounds_c *bounds = ecs_ensure (ecs, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 70.f, 70.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (ecs, ent, box_c);
    box->b_uses_color = false;
    box->b_is_shown = true;
    box->b_is_filled = false;
    click_c *click = ecs_ensure (ecs, ent, click_c);
    click->toggled_r = 255u;
    click->toggled_g = 255u;
    click->toggled_b = 0u;
    ecs_add (ecs, ent, drag_c);
    color_c *color = ecs_ensure (ecs, ent, color_c);
    color->default_r = 0u;
    color->default_g = 0u;
    color->default_b = 0u;
    ecs_add (ecs, ent, hover_c);
    margins_c *margins = ecs_ensure (ecs, ent, margins_c);
    margins->value = (struct margins_ngrid){
      .top = 12.f, .left = 12.f, .bottom = 12.f, .right = 12.f
    };
    origin_c *origin = ecs_ensure (ecs, ent, origin_c);
    origin->relative = (SDL_FPoint){ 100.f, 200.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
    ecs_add (ecs, ent, resize_c);
    text_c *text = ecs_ensure (ecs, ent, text_c);
    string_init_set_str (text->content, "hello, world");
    text->font_size = 4u;
    text->align_h = TEXT_ALIGN_H_LEFT;
    text->align_v = TEXT_ALIGN_V_CENTER;
    text->b_is_shown = true;
    text->b_uses_color = true;
  }

  /* Child box test */
  {
    ecs_entity_t ent = ecs_entity (ecs, { .name = "box2" });
    alpha_c *alpha = ecs_ensure (ecs, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (ecs, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 15.f, 15.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (ecs, ent, box_c);
    box->b_uses_color = true;
    box->b_is_shown = true;
    box->b_is_filled = true;
    click_c *click = ecs_ensure (ecs, ent, click_c);
    click->toggled_r = 255u;
    click->toggled_g = 255u;
    click->toggled_b = 0u;
    color_c *color = ecs_ensure (ecs, ent, color_c);
    color->default_r = 255u;
    color->default_g = 0u;
    color->default_b = 0u;
    ecs_add (ecs, ent, drag_c);
    ecs_add (ecs, ent, hover_c);
    origin_c *origin = ecs_ensure (ecs, ent, origin_c);
    origin->relative = (SDL_FPoint){ 80.f, 80.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
    ecs_add_pair (ecs, ent, EcsChildOf, ecs_lookup (ecs, "box1"));
  }

  /* Mouse cursor test */
  {
    ecs_entity_t ent = ecs_entity (ecs, { .name = "mouse" });
    alpha_c *alpha = ecs_ensure (ecs, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (ecs, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 32.f, 32.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (ecs, ent, box_c);
    box->b_uses_color = true;
    box->b_is_shown = true;
    box->b_is_filled = false;
    color_c *color = ecs_ensure (ecs, ent, color_c);
    color->default_r = 255u;
    color->default_g = 0u;
    color->default_b = 0u;
    origin_c *origin = ecs_ensure (ecs, ent, origin_c);
    origin->relative_callback = get_mouse_position;
    origin->b_can_be_scaled = false;
    origin->b_is_center = false;
    sprite_c *sprite = ecs_ensure (ecs, ent, sprite_c);
    sprite->render_type = SPRITE_RENDER_TYPE_DEFAULT;
    sprite->b_uses_color = false;
    string_init_set_str (sprite->name, "T_Cursor.png");
  }

  /* Animation test */
  {
    ecs_entity_t ent = ecs_entity (ecs, { .name = "animation" });
    anim_player_c *anim_player = ecs_ensure (ecs, ent, anim_player_c);
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
    alpha_c *alpha = ecs_ensure (ecs, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (ecs, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 64.f, 96.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (ecs, ent, box_c);
    box->b_uses_color = false;
    box->b_is_shown = true;
    box->b_is_filled = false;
    color_c *color = ecs_ensure (ecs, ent, color_c);
    color->default_r = 55u;
    color->default_g = 155u;
    color->default_b = 0u;
    origin_c *origin = ecs_ensure (ecs, ent, origin_c);
    origin->relative = (SDL_FPoint){ 40.f, 40.f };
    origin->b_can_be_scaled = false;
    origin->b_is_center = false;
    sprite_c *sprite = ecs_ensure (ecs, ent, sprite_c);
    sprite->render_type = SPRITE_RENDER_TYPE_ANIMATED;
    sprite->b_uses_color = false;
  }

  /* Ngrid test */
  {
    ecs_entity_t ent = ecs_entity (ecs, { .name = "ngrid" });
    alpha_c *alpha = ecs_ensure (ecs, ent, alpha_c);
    alpha->value = 255u;
    bounds_c *bounds = ecs_ensure (ecs, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 300.f, 240.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (ecs, ent, box_c);
    box->b_uses_color = false;
    box->b_is_shown = true;
    box->b_is_filled = false;
    color_c *color = ecs_ensure (ecs, ent, color_c);
    color->default_r = 55u;
    color->default_g = 155u;
    color->default_b = 0u;
    origin_c *origin = ecs_ensure (ecs, ent, origin_c);
    origin->relative = (SDL_FPoint){ 300.f, 40.f };
    origin->b_can_be_scaled = false;
    origin->b_is_center = false;
    margins_c *margins = ecs_ensure (ecs, ent, margins_c);
    margins->value = (struct margins_ngrid){
      .top = 70.f, .left = 70.f, .bottom = 70.f, .right = 70.f
    };
    ngrid_c *ngrid = ecs_ensure (ecs, ent, ngrid_c);
    string_init_set_str (ngrid->name, "T_Ngrid.png");
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
                                           ecs);
            }
          if (e.type == SDL_EVENT_KEY_UP)
            {
              input_man_unregister_scancode (core->input_man, e.key.scancode,
                                             ecs);
            }
          if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
              input_man_try_handle_mouse_motion (core->input_man, e.motion,
                                                 ecs);
            }
          if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
              input_man_try_handle_mouse_down (core->input_man, e.button, ecs);
            }

          if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
              input_man_try_handle_mouse_up (core->input_man, e.button, ecs);
            }
        }
      input_man_bounce_keys (core->input_man, ecs);
      SDL_SetRenderDrawColor (core->rend, 0, 0, 67, 255);
      SDL_RenderClear (core->rend);
      ecs_progress (ecs, 0.f);
      SDL_RenderPresent (core->rend);
    }

  return 0;
}
