#include "SDL3/SDL.h"
#include "flecs.h"

#include "pluto.h"

#include "game_modules/log.h"

Sint32 DEBUG_LOG = DEBUG_LOG_NONE;


int
main (int argc, char *argv[])
{
  ecs_world_t *ecs = ecs_init ();
  SDL_Renderer *r = init_pluto (ecs, (SDL_Point){ 640, 480 });

  ECS_COMPONENT(ecs, bounds_c);
  ECS_COMPONENT(ecs, box_c);
  ECS_COMPONENT(ecs, origin_c);

  ecs_entity_t test_box = ecs_entity(ecs, {.name="box"});
  bounds_c *bounds = ecs_ensure(ecs, test_box, bounds_c);
  bounds->size = (SDL_FPoint){50.f, 50.f};
  bounds->b_can_be_scaled = true;
  box_c *box = ecs_ensure(ecs, test_box, box_c);
  box->b_uses_color = false;
  box->b_is_shown = true;
  box->b_is_filled = true;
  origin_c *origin = ecs_ensure(ecs, test_box, origin_c);
  origin->position = (SDL_FPoint){100.f, 200.f};
  origin->b_can_be_scaled = true;
  origin->b_is_center = false;

  SDL_Event e;
  while(1)
    {
      while(SDL_PollEvent(&e) > 0)
        {
          if(e.type == SDL_EVENT_QUIT)
            {
              SDL_Quit();
              exit(0);
            }
        }

      SDL_SetRenderDrawColor (r, 0, 0, 67, 255);
      SDL_RenderClear (r);
      ecs_progress(ecs, 0.f);
      SDL_RenderPresent (r);
    }

  return 0;
}
