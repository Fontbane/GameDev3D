#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"

#include "gamelocal.h"

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "world.h"
#include "building.h"
#include "spells.h"

extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    Sprite *mouse = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    Entity *agu;
    Particle particle[100];
    Matrix4 skyMat;
    Model *sky;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }
    
    init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);
    
    slog_sync();
    
    entity_system_init(1024);
    
    mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    
    
    agu = agumon_new(vector3d(0 ,0,0));
    if (agu)agu->selected = 1;
    w = world_load("config/testworld.json");
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    player_new(vector3d(-50,0,0));
    
    for (a = 0; a < 100; a++)
    {
        particle[a].position = vector3d(gfc_crandom() * 100,gfc_crandom() * 100,gfc_crandom() * 100);
        particle[a].color = gfc_color(0,0,0,1);
//        particle[a].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
        particle[a].size = 100 * gfc_random();
    }
    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));
    
    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);

        /*if (game.state == Selecting) {
            game.selection.x = mousex;
            game.selection.y = mousey;
            if (gfc_input_command_down("click")) {
                game.location_chosen = vector3d(mousex, mousey, 0);
                if (B_check_location(game.selection)) {
                    B_spawn(game.location_chosen, game.name);
                    game.state = Day;
                }
            }
        }
        else if (game.state == Day) {
            if (gfc_input_command_down("click")) {
                game.location_chosen = vector3d(mousex, mousey, 0);
                B_at_location(game.location_chosen);
            }
            else if (gfc_input_command_down("cannon")) {
                B_try_buy("cannon");
            }
            else if (gfc_input_command_down("mortar")) {
                B_try_buy("mortar");
            }
            else if (gfc_input_command_down("wiztower")) {
                B_try_buy("wiztower");
            }
            else if (gfc_input_command_down("rocket")) {
                B_try_buy("rocket");
            }
            else if (gfc_input_command_down("infernotower")) {
                B_try_buy("inferno");
            }
        }
        else if (game.state == Night) {
            game.location_chosen = vector3d(mousex, mousey, 0);
            if (game.labBroken == 0) {
                if (gfc_input_command_down("freeze_spell")) {
                    S_try_buy(game.location_chosen, "freeze_spell");
                }
                else if (gfc_input_command_down("heal_spell")) {
                    S_try_buy(game.location_chosen, "heal_spell");
                }
                else if (gfc_input_command_down("sludge_spell")) {
                    S_try_buy(game.location_chosen, "sludge_spell");
                }
                else if (gfc_input_command_down("smite_spell")) {
                    S_try_buy(game.location_chosen, "smite_spell");
                }
                else if (gfc_input_command_down("pyro_spell")) {
                    S_try_buy(game.location_chosen, "pyro_spell");
                }
            }
        }
        else if (game.state == Selected) {
            if (game.selected) {
                Entity* building = (Entity*)game.selected;
                if (gfc_input_command_down("rebuild")) {
                    B_try_rebuild(building);
                }
                else if (gfc_input_command_down("choose_one")) {
                    B_apply_upgrade(building, 1);
                }
                else if (gfc_input_command_down("choose_two")) {
                    B_apply_upgrade(building, 2);
                }
            }
        }
        else if (game.state = Over) {
            Box box = {
                .x = 0,
                .y = 0,
                .z = 0,
                .w = 10,
                .h = 10,
                .d = 10
            };
            gf3d_draw_cube_solid(box, vector3d(0,0,0), vector3d(0,0,0), vector3d(1000, 1000, 1000),
                gfc_color(SDL_GetTicks()%2, SDL_GetTicks()%3, SDL_GetTicks()%4, 1));
        }*/
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);
        entity_think_all();
        entity_update_all();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                world_draw(w);
                entity_draw_all();
                
                for (a = 0; a < 100; a++)
                {
                    gf3d_particle_draw(&particle[a]);
                }
            //2D draws
                gf2d_draw_rect_filled(gfc_rect(10, 10, 1000, 32), gfc_color8(128, 128, 128, 255));
                gf2d_font_draw_line_tag("Press ALT+F4 to exit",FT_H1,gfc_color(1,1,1,1), vector2d(10,10));
                
                gf2d_draw_rect(gfc_rect(10 ,10,1000,32),gfc_color8(255,255,255,255));
                
                gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(8,8,0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);
        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))done = 1; // exit condition
    }    
    
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
