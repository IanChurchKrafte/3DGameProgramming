#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_primitives.h"

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

#include "entity.h"
#include "agumon.h"
#include "monster1.h"
#include "monster2_kong.h"
#include "monster3_porygon.h"
#include "monster4_skelly.h"
#include "monster5_mario.h"
#include "monster6_yoshi.h"
#include "monster7_creeper.h"
#include "monster8_finn.h"
#include "monster9_goomba.h"
#include "monster10_arlo.h"
#include "defense1_smallFence.h"
#include "defense2_smallWall.h"
#include "defense3_turret1.h"
#include "defense4_turret2.h"
#include "defense5_turret3.h"
#include "player.h"
#include "world.h"

extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    //Sprite *mouse = NULL;
    Sprite *crosshair = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    //Entity *agu;
    Entity *charmander = NULL, *kong = NULL, *porygon = NULL, *skelly = NULL, 
           *mario = NULL, *yoshi = NULL, *creeper = NULL, *finn = NULL, *goomba = NULL, *arlo = NULL;
    Entity *fence = NULL, *wall = NULL, *turret1 = NULL, *turret2 = NULL, *turret3 = NULL;
    //Particle particle[100];
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
    
    //1024 max entites
    entity_system_init(1024);
    
    //mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    crosshair = gf2d_sprite_load("images/crosshair.png",32,32,16);
    
    //agu = agumon_new(vector3d(0,0,0));
    //if (agu)agu->selected = 1;
    //charmander = monster1_new(vector3d(15,0,-20));
    //if (charmander)charmander->selected = 1;
    //Vector3D default = vector3d(-3.0, 0, 0);
    //spawn in sample characters
    charmander = monster1_new(vector3d(-40, -35, -25));
    kong = monster2_kong_new(vector3d(-40, -27, -26));
    porygon = monster3_porygon_new(vector3d(-40, -20, -25));
    skelly = monster4_skelly_new(vector3d(-40, -13, -25));
    mario = monster5_mario_new(vector3d(-40, -6, -27));
    // yoshi = monster6_yoshi_new(vector3d(-40, 1, -27));
    // creeper = monster7_creeper_new(vector3d(-40, 8, -25));
    // finn = monster8_finn_new(vector3d(-40, 15, -24));
    // goomba = monster9_goomba_new(vector3d(-40, 22, -27));
    // arlo = monster10_arlo_new(vector3d(-40, 30, -25));
    fence = defense1_smallFence_new(vector3d(-30, -40, -28), vector3d(-3.0, 0, 0));
    fence->selected = 0;
    wall = defense2_smallWall_new(vector3d(-15, -40, -28), vector3d(-3.0, 0, 0));
    wall->selected = 0;
    turret1 = defense3_turret1_new(vector3d(0, -40, -28), vector3d(-3.0, 0, 0));
    turret1->selected = 0;
    turret2 = defense4_turret2_new(vector3d(15, -40, -28), vector3d(-3.0, 0, 0));
    turret2->selected = 0;
    turret3 = defense5_turret3_new(vector3d(30, -40, -28), vector3d(-3.0, 0, 0));
    turret3->selected = 0;
    //list for easier freeing later
    Entity *entList[15] = {charmander, kong, porygon, skelly, mario, yoshi, creeper, finn, goomba, arlo, fence, wall, turret1, turret2, turret3};


    w = world_load("config/world.json");
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    Entity* player = player_new(vector3d(-30,0,-20));
    //player_new(vector3d(10,10,0));
    
    /*
    for (a = 0; a < 100; a++)
    {
        particle[a].position = vector3d(gfc_crandom() * 100,gfc_crandom() * 100,gfc_crandom() * 100);
        particle[a].color = gfc_color(0,0,0,1);
//        particle[a].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
        particle[a].size = 100 * gfc_random();
    }
    a = 0;
    */
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));

    //set floor
    //VT_SPHERE sphere = gfc_plane3d(0, 0, -30, 30);
    //Plane3D bottom = gfc_plane3d(0, 0, -30, 30);
    // main game loop
    slog("gf3d main loop begin");
    //char* point = 100+'0';
    //char point[32] = "Points: ";
    //Entity *defenseList;
    while(!done)
    {
        
        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);
        
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);
        entity_think_all();
        //player_update(player, player);
        entity_update_all(player);
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                world_draw(w);
                entity_draw_all();
                
                /*
                for (a = 0; a < 100; a++)
                {
                    gf3d_particle_draw(&particle[a]);
                }
                */
            //2D draws
                //setting up ui to be drawn
                char ui[64];
                char attack[7];
                unsigned short points = player->points;
                unsigned short health = player->health;
                unsigned short damage = player->attackDamage;
                int attackType = player->attackType;
                //slog("attack: %i", attackType);
                switch(attackType){
                    case 0:
                        strncpy(attack, "bullet", 7);
                        break;
                    case 1:
                        strncpy(attack, "fire", 7);
                        break;
                    case 2:
                        strncpy(attack, "melee", 7);
                        break;
                    case 3:
                        strncpy(attack, "magic", 7);
                        break;
                    case 4:
                        strncpy(attack, "ice", 7);
                        break;
                }
                sprintf(ui, "Points: %hi     Health: %hi     Attack: %s    Damage: %hi", points, health, attack, damage);
                

                gf2d_draw_rect_filled(gfc_rect(10 ,10,1000,32),gfc_color8(128,128,128,255));
                
                /*
                could try representing the points with a bar graph

                or doing atomic variables with multi-threading so its a seperate thread doing the points
                */
                gf2d_font_draw_line_tag(ui,FT_H1,gfc_color(1,1,1,1), vector2d(10,10));
                //free(point);
                gf2d_draw_rect(gfc_rect(10 ,10,1000,32),gfc_color8(255,255,255,255));
                
                gf2d_sprite_draw(crosshair, vector2d(240,320), vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                //gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(8,8,0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);
                
                //slog("Time Elapsed: %u", currentTime-lastTime);
        //unsigned int lastTime = SDL_GetTicks(), currentTime;
        gf3d_vgraphics_render_end(); //where the lag is coming from
        //currentTime = SDL_GetTicks();
        //slog("Time Elapsed: %u", currentTime-lastTime);
        if (gfc_input_command_down("exit"))done = 1; // exit condition

        //currentTime = SDL_GetTicks();
        //slog("Time Elapsed: %u", currentTime-lastTime);
    }    
    //free sample entities
    for(int i=0; i<10; i++){
        if(entList[i] != NULL)
            entity_free(entList[i]);
    }
    //entity_free(creeper);
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
