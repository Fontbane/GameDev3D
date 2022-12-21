#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"
#include "gf3d_draw.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gf2d_mouse.h"

#include "gamelocal.h"
#include "save.h"

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "world.h"
#include "enemy.h"
#include "building.h"
#include "spells.h"

extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    //Uint32 then;
    World *w;
    Particle particle[100];
    Matrix4 skyMat;
    Model *sky;
    //Entity* agu;

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
    gf3d_draw_init();
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);

    gf2d_actor_init(256);

    gf2d_mouse_load("actors/mouse.actor");
    
    slog_sync();
    
    entity_system_init(1024);
    B_init_statmap();
    M_statmap_init();
    S_statmap_init();
    gfc_audio_init(512, 16, 8, 8, true, true);
    
    
    w = world_load("config/testworld.json");
    //agu = agumon_new(vector3d(0, 0, 0));
    //agu->selected = 1;
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    player_new(vector3d(0,256,0));
    
    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));
    game.state = MainMenu;
    game.hutBroken = 0;
    game.labBroken = 0;
    saveData = (PlayerData*)malloc(sizeof(PlayerData));
    saveData->day = 0;
    saveData->gold = 200;
    saveData->elixir = 0;
    saveData->enemiesKilled = 0;
    
    //gf3d_camera_look_at(vector3d(0, -50, 0), vector3d(0, 0, 0), vector3d(0, 0, 1));
    game.time = SDL_GetTicks();

    Sound* bgm_day = gfc_sound_load("audio/day.wav", 1, 1);
    Sound* bgm_night = gfc_sound_load("audio/night.wav", 1, 1);
    gfc_sound_play(bgm_day, 2, 1, 1, -1);
    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        gfc_input_update();
        gf2d_font_update();
        gf2d_mouse_update();

        Uint32 t = SDL_GetTicks()+1;
        if (t-game.time>36000 && game.state == Day) {
            game.state = Night;
            game.time = t;
            game.weight = 0;
            M_spawn(vector3d(0, 0, 100), "buster");
            M_spawn(vector3d(20, 0, 100), "golem");
            M_spawn(vector3d(-20, 0, 100), "bat");
            M_spawn(vector3d(40, 0, 100), "archer");
            M_spawn(vector3d(-40, 0, 100), "dragon");
            slog("Switching to night");
            gfc_sound_play(bgm_night, 2, 1, 1, -1);
        }
        else if (t - game.time > 36000 && game.state == Night) {
            game.state = Day;
            game.time = t;
            slog("Switching to day");
            saveData->day++;
            gfc_sound_play(bgm_day, 2, 1, 1, -1);
        }
        
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
            //2D draws
                gf2d_draw_rect_filled(gfc_rect(10, 10, 1080, 32), gfc_color8(128, 128, 128, 255));
                
                gf2d_draw_rect(gfc_rect(10 ,10,1080,32),gfc_color8(255,255,255,255));

                if (game.state == MainMenu) {
                    gf2d_font_draw_line_tag("Crash of Crams", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 10));
                    gf2d_font_draw_line_tag("Press ENTER to start", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 200));
                    if (gfc_input_command_down("click")) {
                        slog("Starting game");
                        game.state = Day;
                        saveData->day = 0;
                        B_spawn(vector3d(0, 0, 0), HALL);
                        B_spawn(vector3d(-4, 0, 0), HUT);
                        B_spawn(vector3d(4, 0, 0), LAB);
                    }
                }
                else if (game.state == Selecting) {
                    gf2d_font_draw_line_tag("Press ENTER to select where to place the building", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 10));
                    char gold[32];
                    _snprintf(gold, 32, "Gold %i Elixir %i", saveData->gold, saveData->elixir);
                    gf2d_font_draw_line_tag(gold, FT_H1, gfc_color(1, 1, 1, 1), vector2d(320, 100));
                    gf3d_draw_cube_solid(game.selection, game.location_chosen, vector3d(0, 0, 0), vector3d(1, 1, 1), gfc_color(1, 0, 0, 0.5));
                    if (gfc_input_command_down("click")) {
                        slog("Checking this location...");
                        //if (B_check_location(game.selection)) {
                            B_spawn(game.location_chosen, game.name);
                            game.state = Day;
                        //}
                    }
                }
                else if (game.state == Day) {
                    gf2d_font_draw_line_tag("[c]annon 300 [m]ortar 800 [r]ocket 1000 [w]izard tower 4000 [i]nferno tower 12000", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 10));
                    gf2d_font_draw_line_tag("[g]old mine 200 [l]ey line 200 | press ENTER to select building", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 50));
                    char day[32];
                    _snprintf(day, 32, "Day %i", saveData->day);
                    gf2d_font_draw_line_tag(day, FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 100));
                    char gold[32];
                    _snprintf(gold, 32, "Gold %i Elixir %i", saveData->gold, saveData->elixir);
                    gf2d_font_draw_line_tag(gold, FT_H1, gfc_color(1, 1, 1, 1), vector2d(320, 100));
                    if (gfc_input_command_down("rightclick")) {
                        B_at_location(game.location_chosen);
                    }
                    else if (gfc_input_command_down("cannon")) {
                        B_try_buy(CANNON);
                    }
                    else if (gfc_input_command_down("mortar")) {
                        B_try_buy(MORTAR);
                    }
                    else if (gfc_input_command_down("wiztower")) {
                        B_try_buy(WIZARD_TOWER);
                    }
                    else if (gfc_input_command_down("rocket")) {
                        B_try_buy(ROCKETS);
                    }
                    else if (gfc_input_command_down("infernotower")) {
                        B_try_buy(INFERNO_TOWER);
                    }
                    else if (gfc_input_command_down("mine")) {
                        B_try_buy(MINE);
                    }
                    else if (gfc_input_command_down("ley")) {
                        B_try_buy(LEY);
                    }
                }
                else if (game.state == Night) {
                    gf2d_font_draw_line_tag("[f]reeze 300 [h]eal 800 [s]ludge 1000 [x]smite 4000 [p]yro 12000", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 10));
                    char day[32];
                    _snprintf(day, 32, "Night %i", saveData->day);
                    gf2d_font_draw_line_tag(day, FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 100));
                    char gold[32];
                    _snprintf(gold, 32, "Gold %i Elixir %i", saveData->gold, saveData->elixir);
                    gf2d_font_draw_line_tag(gold, FT_H1, gfc_color(1, 1, 1, 1), vector2d(320, 100));
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
                    gf2d_font_draw_line_tag("[o] rebuild [1] upgrade 1 [2] upgrade 2", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 10));
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
                        else if (gfc_input_command_down("options")) {
                            game.state=Day;
                            game.selected = NULL;
                        }
                    }
                    else {
                        game.state = Day;
                    }
                }
                else if (game.state == Over) {
                    gf2d_draw_rect_filled(gfc_rect(0, 0, 1440, 1440), gfc_color8(SDL_GetTicks() % 2 * 255, 0, !(SDL_GetTicks() % 2) * 255, 255));
                }
                gf2d_mouse_draw();
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
