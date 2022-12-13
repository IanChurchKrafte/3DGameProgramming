#include <unistd.h>
#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_primitives.h"
#include "gfc_audio.h"

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

void saveWorld(World *w);
void loadWorld();

//function is buggy and gets stuck on the first frame
bossGifFun(int lastFrame, Vector2D pos, Sprite *bossGif[20], Entity *player);
void playDeathMusic();

int deadMusicPlaying = 0;
Sound *deadMusic = NULL;

int main(int argc,char *argv[])
{
    int done = 0, fin = 0, new = 0, load = 0;
    int a;
    
    Sprite *mouse = NULL;
    Sprite *crosshair = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    //Entity *agu;
    // Entity *charmander = NULL, *kong = NULL, *porygon = NULL, *skelly = NULL, 
    //        *mario = NULL, *yoshi = NULL, *creeper = NULL, *finn = NULL, *goomba = NULL, *arlo = NULL;
    // Entity *fence = NULL, *wall = NULL, *turret1 = NULL, *turret2 = NULL, *turret3 = NULL;
    //Particle particle[100];
    //Entity *creeper = NULL;
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
    //gfc_audio_init(256, 16, 4, 1, 1, 1);
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);
    
    slog_sync();

    //Before loading world or entities the main menu will be shown
    //They will have the options of new game or load game
    //Load game will load it from a JSON
    //start main menu
    
    
    //1024 max entites
    entity_system_init(1024);
    
    //mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    crosshair = gf2d_sprite_load_image("images/crosshair.png");
    
    //agu = agumon_new(vector3d(0,0,0));
    //if (agu)agu->selected = 1;
    //charmander = monster1_new(vector3d(15,0,-20));
    //if (charmander)charmander->selected = 1;
    //Vector3D default = vector3d(-3.0, 0, 0);
    //spawn in sample characters
    // charmander = monster1_new(vector3d(-40, -35, -25));
    // kong = monster2_kong_new(vector3d(-40, -27, -26));
    // porygon = monster3_porygon_new(vector3d(-40, -20, -25));
    // skelly = monster4_skelly_new(vector3d(-40, -13, -25));
    // mario = monster5_mario_new(vector3d(-40, -6, -27));
    // yoshi = monster6_yoshi_new(vector3d(-40, 1, -27));
    // creeper = monster7_creeper_new(vector3d(-40, 8, -25));
    // finn = monster8_finn_new(vector3d(-40, 15, -24));
    // goomba = monster9_goomba_new(vector3d(-40, 22, -27));
    // arlo = monster10_arlo_new(vector3d(-40, 30, -25));
    // fence = defense1_smallFence_new(vector3d(-30, -40, -28), vector3d(-3.0, 0, 0));
    // fence->selected = 0;
    // wall = defense2_smallWall_new(vector3d(-15, -40, -28), vector3d(-3.0, 0, 0));
    // wall->selected = 0;
    // turret1 = defense3_turret1_new(vector3d(0, -40, -28), vector3d(-3.0, 0, 0));
    // turret1->selected = 0;
    // turret2 = defense4_turret2_new(vector3d(15, -40, -28), vector3d(-3.0, 0, 0));
    // turret2->selected = 0;
    // turret3 = defense5_turret3_new(vector3d(30, -40, -28), vector3d(-3.0, 0, 0));
    // turret3->selected = 0;
    //list for easier freeing later
    //Entity *entList[15] = {charmander, kong, porygon, skelly, mario, yoshi, creeper, finn, goomba, arlo, fence, wall, turret1, turret2, turret3};


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


    //setup audio
    //background game sound for when in the main menu, loops 10 times
    gfc_audio_init(256, 16, 4, 1, 1, 1);
    Sound *menuMusic = gfc_sound_load("music/209561__dneproman__8-bit-style.wav", 1.0, 1);
    deadMusic = gfc_sound_load("music/esm_8_bit_game_over_1_arcade_80s_simple_alert_notification_game.mp3", 1.0, 1);
    Sound *backgroundMusic = gfc_sound_load("music/415804__sunsai__mushroom-background-music.wav", 1.0, 1);
    
    //setup main menu
    //will be a sprite with buttons to click, new game, load game, and exit
    //main game loop wont start until new game or load game is clicked
    //will be 4 sprites, the background image of the main menu, and the 3 buttons
    int startGame = 0; //will turn to 1 when player chooses
    Sprite *mainMenu = NULL, *newGame = NULL, *loadGame = NULL, *exitGame = NULL, *editMode = NULL, *youDied = NULL, *monsterSpawns = NULL, *defenseSpawns = NULL;
    mainMenu = gf2d_sprite_load_image("images/mainMenu/faith-spark-background1.png");
    newGame = gf2d_sprite_load_image("images/mainMenu/newGame.png");
    loadGame = gf2d_sprite_load_image("images/mainMenu/loadGame.png");
    exitGame = gf2d_sprite_load_image("images/mainMenu/quitGame.png");
    editMode = gf2d_sprite_load_image("images/mainMenu/editGame.png");
    youDied = gf2d_sprite_load_image("images/you-died.png");
    monsterSpawns = gf2d_sprite_load_image("images/editMode_MonsterSpawns-clear.png");
    defenseSpawns = gf2d_sprite_load_image("images/editMode_DefenseSpawns-clear.png");
    


    mouse = gf2d_sprite_load_image("images/mainMenu/mouse.png");

    Sprite *bossGif[20];
    bossGif[0] = gf2d_sprite_load_image("images/boss/boss1.png");
    bossGif[1] = gf2d_sprite_load_image("images/boss/boss2.png");
    bossGif[2] = gf2d_sprite_load_image("images/boss/boss3.png");
    bossGif[3] = gf2d_sprite_load_image("images/boss/boss4.png");
    bossGif[4] = gf2d_sprite_load_image("images/boss/boss5.png");
    bossGif[5] = gf2d_sprite_load_image("images/boss/boss6.png");
    bossGif[6] = gf2d_sprite_load_image("images/boss/boss7.png");
    bossGif[7] = gf2d_sprite_load_image("images/boss/boss8.png");
    bossGif[8] = gf2d_sprite_load_image("images/boss/boss9.png");
    bossGif[9] = gf2d_sprite_load_image("images/boss/boss10.png");
    bossGif[10] = gf2d_sprite_load_image("images/boss/boss11.png");
    bossGif[11] = gf2d_sprite_load_image("images/boss/boss12.png");
    bossGif[12] = gf2d_sprite_load_image("images/boss/boss13.png");
    bossGif[13] = gf2d_sprite_load_image("images/boss/boss14.png");
    bossGif[14] = gf2d_sprite_load_image("images/boss/boss15.png");
    bossGif[15] = gf2d_sprite_load_image("images/boss/boss16.png");
    bossGif[16] = gf2d_sprite_load_image("images/boss/boss17.png");
    bossGif[17] = gf2d_sprite_load_image("images/boss/boss18.png");
    bossGif[18] = gf2d_sprite_load_image("images/boss/boss19.png");
    bossGif[19] = gf2d_sprite_load_image("images/boss/boss20.png");
    //draw main menu
    Vector2D bossBannerPostion = vector2d(680, 10);

    int lastSpawn = 0;
    gfc_sound_play(menuMusic, 10, 1.0, 1, 1);
    while(!fin){
        gf3d_vgraphics_render_start();

        gfc_input_update();

        SDL_GetMouseState(&mousex,&mousey);
        
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;

        // SDL_Event e;
        // int buttonDown = 0;
        // while(SDL_PollEvent(&e)){
        //     slog("in poll event: %s", e.type);
        //     switch (e.type){
        //         case 4:
        //             buttonDown = 1;
        //             slog("buttonDown");
        //             break;
        //     }
        // }

        const Uint8 * keys;
        keys = SDL_GetKeyboardState(NULL);
        

        gf2d_sprite_draw(mainMenu, vector2d(0,0), vector2d(2,2), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
        gf2d_sprite_draw(newGame, vector2d(800,320), vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
        gf2d_sprite_draw(loadGame, vector2d(800,460), vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
        gf2d_sprite_draw(editMode, vector2d(800,620), vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
        gf2d_sprite_draw(exitGame, vector2d(800,780), vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
        
        

        //gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);



        if(keys[SDL_SCANCODE_N] || (mousex >= 800 && mousex<=1150 && mousey>=320 && mousey<=380)){
            //start new game
            fin = 1;
            new = 1;
            player->editMode = 0;
            player->points = 0;
            slog("new game attempt");
        }
        if(keys[SDL_SCANCODE_L] || (mousex >= 800 && mousex<=1150 && mousey>=460 && mousey<=520)){
            //load game
            fin = 1;
            load = 1;
            loadEntities(player);
            player->editMode = 0;
            slog("load game attempt");
        }
        if(keys[SDL_SCANCODE_Q] || (mousex >= 800 && mousex<=1150 && mousey>=780 && mousey<=810)){
            fin = 1;
            done = 1;
            slog("exit game attempt");
            //exit game
        }
        if(keys[SDL_SCANCODE_E] || (mousex >= 800 && mousex<=1150 && mousey>=620 && mousey<=650)){
            slog("edit mode");
            player->editMode = 1;
            fin = 1;
            new = 1;
            player->points = 50000;
        }

        gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(0.5,0.5),vector3d(8,8,0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);

        gf3d_vgraphics_render_end();

    }
    gfc_sound_play(menuMusic, 0, 0.001, 1, 1);
    gf2d_sprite_free(mainMenu);
    gf2d_sprite_free(newGame);
    gf2d_sprite_free(loadGame);
    gf2d_sprite_free(editMode);
    gf2d_sprite_free(exitGame);
    gfc_sound_free(menuMusic);




    //set floor
    //VT_SPHERE sphere = gfc_plane3d(0, 0, -30, 30);
    //Plane3D bottom = gfc_plane3d(0, 0, -30, 30);
    // main game loop
    slog("gf3d main loop begin");

    gfc_sound_play(backgroundMusic, 100, 0.5, -1, 1);

    int dead = 0;
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
                // Shape box;
                // box.s.r.h = creeper->bounds.h;
                // box.s.r.w = creeper->bounds.w;
                // box.s.r.x = creeper->bounds.x;
                // box.s.r.y = creeper->bounds.y;
                // box.type = ST_RECT;
                
                //gf2d_draw_shape(box, gfc_color(0,1,0,1), vector2d(0,0));
                
                
                /*
                for (a = 0; a < 100; a++)
                {
                    gf3d_particle_draw(&particle[a]);
                }
                */
            //2D draws
                /*
                    could try representing the points with a bar graph
                    or doing atomic variables with multi-threading so its a seperate thread doing the points
                */
                //setting up ui to be drawn

                if(player->editMode == 1){
                    gf2d_sprite_draw(monsterSpawns, vector2d(0,50), vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                    gf2d_sprite_draw(defenseSpawns, vector2d(1550,50), vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                }

                char ui[64];
                char attack[7];
                unsigned int points = player->points;
                unsigned short health = player->health;
                unsigned short damage = player->attackDamage;
                unsigned short currentWave = player->currentWave;
                int attackType = player->attackType;
                //slog("attack: %i", attackType);
                switch(attackType){
                    case 0:
                        strncpy(attack, "bullet", 7);
                        break;
                    case 1:
                        strncpy(attack, "fire", 5);
                        break;
                    case 2:
                        strncpy(attack, "melee", 6);
                        break;
                    case 3:
                        strncpy(attack, "magic", 6);
                        break;
                    case 4:
                        strncpy(attack, "ice", 4);
                        break;
                }
                //for testing
                // float x = player->position.x;
                // float y = player->position.y;
                // float z = player->position.z;
                // char pos[32];
                // sprintf(pos, "X: %f, Y: %f, z: %f", x, y, z);


                //ui draw
                sprintf(ui, "Points: %u     Health: %hi     Attack: %s    Damage: %hi   Wave: %hi", points, health, attack, damage, currentWave);

                gf2d_draw_rect_filled(gfc_rect(10,10,1000,40),gfc_color8(128,128,128,255));
                

                gf2d_font_draw_line_tag(ui,FT_H1,gfc_color(1,1,1,1), vector2d(10,12.5));

                
                //free(point);
                gf2d_draw_rect(gfc_rect(10 ,10,1000,40),gfc_color8(255,0,0,255));
                
                //for testing
                // gf2d_font_draw_line_tag(pos,FT_H1,gfc_color(0,0,0,1), vector2d(10,30)); 
                gf2d_sprite_draw(crosshair, vector2d(900,510), vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                //gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(8,8,0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);

                //the only way that seem to work to go through each frame of the gif with the right timing, each time it goes into the next iteration of the game loop it will go to the next frame of the gif
                //looks bad but it works perfectly
                if(player->bossSpawning == 1){
                    if(lastSpawn == 0){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 1){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 2){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 3){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 4){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 5){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 6){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 7){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 8){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 9){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 10){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 11){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 12){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 13){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 14){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 15){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 16){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 17){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 18){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn++;
                    }
                    else if(lastSpawn == 19){
                        gf2d_sprite_draw(bossGif[lastSpawn], bossBannerPostion, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                        lastSpawn = 0;
                        player->bossSpawning=0;
                    }
                    // bossGifFun(lastSpawn, bossBannerPostion, bossGif, player);
                    //gf2d_draw_rect(box.s.r, gfc_color(0,1,0,1));
                }

                if (player->isDead == 1){
                    //slog("trying to draw you died sprite");
                    gf2d_sprite_draw(youDied, vector2d(0,0), vector2d(2,2), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
                    dead++;
                    slog("You Died. Closing game in %i", 60-dead);
                    playDeathMusic();
                }
                if(dead == 60) done=1;
                
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
    // for(int i=0; i<10; i++){
    //     if(entList[i] != NULL)
    //         entity_free(entList[i]);
    // }
    //entity_free(creeper);

    //save world data
    saveWorld(w);
    
    world_delete(w);

    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

void saveWorld(World *w){
    SJson *body = sj_object_new();
    // SJson *modelMat = sj_object_new();
    SJson *position = sj_object_new();
    SJson *rotation = sj_object_new();
    SJson *scale = sj_object_new();
    SJson *model = sj_object_new();
    // SJson *color = sj_object_new();
    // SJson *spawnList = sj_object_new();
    // SJson *entityList = sj_object_new();
    sj_object_insert(position, "position_x", sj_new_float(w->position.x));
    sj_object_insert(position, "position_y", sj_new_float(w->position.y));
    sj_object_insert(position, "position_z", sj_new_float(w->position.z));
    sj_object_insert(body, "position", position);

    sj_object_insert(rotation, "rotation_x", sj_new_float(w->rotation.x));
    sj_object_insert(rotation, "rotation_y", sj_new_float(w->rotation.y));
    sj_object_insert(rotation, "rotation_z", sj_new_float(w->rotation.z));
    sj_object_insert(body, "rotation", rotation);

    sj_object_insert(scale, "scale_x", sj_new_float(w->scale.x));
    sj_object_insert(scale, "scale_y", sj_new_float(w->scale.y));
    sj_object_insert(scale, "scale_z", sj_new_float(w->scale.z));
    sj_object_insert(body, "scale", scale);

    sj_object_insert(model, "modelLocation", sj_new_str("models/world.model"));
    sj_object_insert(body, "model", model);

    sj_save(body, "saves/wordSave.json");
}

void loadWorld(World *w){
    SJson *body = sj_object_new();
    body = sj_load("saves/worldSave.json");

    SJson *position = sj_object_new();
    SJson *rotation = sj_object_new();
    SJson *scale = sj_object_new();
    SJson *model = sj_object_new();
    
    position = sj_object_get_value(body, "position");
    sj_get_float_value(sj_object_get_value(position, "position_x"), &w->position.x);
    sj_get_float_value(sj_object_get_value(position, "position_y"), &w->position.y);
    sj_get_float_value(sj_object_get_value(position, "position_z"), &w->position.z);

    rotation = sj_object_get_value(body, "rotation");
    sj_get_float_value(sj_object_get_value(rotation, "rotation_x"), &w->rotation.x);
    sj_get_float_value(sj_object_get_value(rotation, "rotation_y"), &w->rotation.y);
    sj_get_float_value(sj_object_get_value(rotation, "rotation_z"), &w->rotation.z);

    scale = sj_object_get_value(body, "scale");
    sj_get_float_value(sj_object_get_value(scale, "scale_x"), &w->scale.x);
    sj_get_float_value(sj_object_get_value(scale, "scale_y"), &w->scale.y);
    sj_get_float_value(sj_object_get_value(scale, "scale_z"), &w->scale.z);

    model = sj_object_get_value(body, "model");
    char *str = sj_get_string_value(sj_object_get_value(model, "model"));
    //&w->model->filename=str;

}

int bossGifFun(int lastFrame, Vector2D pos, Sprite *bossGif[20], Entity *player){
    gf2d_sprite_draw(bossGif[lastFrame], pos, vector2d(1,1), vector3d(0,0,0), gfc_color(1,1,1,1), 1);
    if(lastFrame == 19){
        player->bossSpawning = 0;
        return 0;
    }
    else
        return lastFrame++;
}

void playDeathMusic(){
    if(deadMusicPlaying == 0){
        gfc_sound_play(deadMusic, 0, 1, 1, 1);
        deadMusicPlaying = 1;
    }
}

/*eol@eof*/
