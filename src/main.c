#include "SDL3/SDL.h"
#include "flecs.h"

#include "pluto.h"

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
}
void
handle_mouse_release (struct input_man *input_man, SDL_FPoint pos,
                      Uint8 button, void *param)
{
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
}

int
main (int argc, char *argv[])
{
  ecs_world_t *ecs = ecs_init ();
  app_s *app = init_pluto (ecs, (SDL_Point){ 640, 480 });

  ECS_COMPONENT (ecs, alpha_c);
  ECS_COMPONENT (ecs, bounds_c);
  ECS_COMPONENT (ecs, box_c);
  ECS_COMPONENT (ecs, click_c);
  ECS_COMPONENT (ecs, color_c);
  ECS_COMPONENT (ecs, hover_c);
  ECS_COMPONENT (ecs, origin_c);
  ECS_COMPONENT (ecs, text_c);

  /* Test box */
  {
    ecs_entity_t ent = ecs_entity (ecs, { .name = "box1" });
    bounds_c *bounds = ecs_ensure (ecs, ent, bounds_c);
    bounds->size = (SDL_FPoint){ 50.f, 50.f };
    bounds->b_can_be_scaled = true;
    box_c *box = ecs_ensure (ecs, ent, box_c);
    box->b_uses_color = false;
    box->b_is_shown = true;
    box->b_is_filled = true;
    color_c *color = ecs_ensure (ecs, ent, color_c);
    color->default_r = 0u;
    color->default_g = 0u;
    color->default_b = 0u;
    origin_c *origin = ecs_ensure (ecs, ent, origin_c);
    origin->relative = (SDL_FPoint){ 100.f, 200.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
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
    ecs_add (ecs, ent, hover_c);
    origin_c *origin = ecs_ensure (ecs, ent, origin_c);
    origin->relative = (SDL_FPoint){ 60.f, 60.f };
    origin->b_can_be_scaled = true;
    origin->b_is_center = false;
    ecs_add_pair (ecs, ent, EcsChildOf, ecs_lookup (ecs, "box1"));
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
              input_man_register_scancode (app->input_man, e.key.scancode,
                                           ecs);
            }
          if (e.type == SDL_EVENT_KEY_UP)
            {
              input_man_unregister_scancode (app->input_man, e.key.scancode,
                                             ecs);
            }
          if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
              input_man_try_handle_mouse_motion (app->input_man, e.motion,
                                                 ecs);
            }
          if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
              input_man_try_handle_mouse_down (app->input_man, e.button, ecs);
            }

          if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
              input_man_try_handle_mouse_up (app->input_man, e.button, ecs);
            }
        }
      input_man_bounce_keys (app->input_man, ecs);
      SDL_SetRenderDrawColor (app->rend, 0, 0, 67, 255);
      SDL_RenderClear (app->rend);
      ecs_progress (ecs, 0.f);
      SDL_RenderPresent (app->rend);
    }

  return 0;
}
