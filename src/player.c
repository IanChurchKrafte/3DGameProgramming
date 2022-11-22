/*
Keep track of all spawned entities in a list
when checking if raycast hits a box just go through that list
this also helps incase multiple of same entity is spawned its not lost by setting the pointer again
do dmg check for dmg reduction

*/

#include "simple_logger.h"
#include "simple_json.h"
#include "simple_json_object.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
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
#include "slowGoo.h"
#include "fastGoo.h"
#include "dmgGoo.h"
#include "healGoo.h"
#include "incDmgGoo.h"
#include "defense1_smallFence.h"
#include "defense2_smallWall.h"
#include "defense3_turret1.h"
#include "defense4_turret2.h"
#include "defense5_turret3.h"
#include "collision.h"
#include "saveGame.h"

#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#include "gfc_primitives.h"
//#include "gfc_vector.h"

#include <stdbool.h>
#include <time.h>
#include <inttypes.h>


int i=0;

Entity *entityList[512];
static int thirdPersonMode = 0;
int points = 0;
Entity *slowGoo = NULL, *fastGoo = NULL, *healGoo = NULL, *dmgGoo = NULL, *incDmgGoo = NULL;
Entity *fence, *wall, *turret1, *turret2, *turret3;
//Entity *defenseList;
int defenseCount = 0; //to keep track of how many defense has spawned for collision
Entity *monster1 = NULL, *monster2 = NULL, *monster3 = NULL, *monster4 = NULL, *monster5 = NULL, 
       *monster6 = NULL, *monster7 = NULL, *monster8 = NULL, *monster9 = NULL, *monster10 = NULL;
void player_think(Entity *self);
void player_update(Entity *self, Entity *player);
void player_damage(int damage, Entity *self, int heal, Entity *inflictor);
void player_death(Entity *self);
bool jump(Entity *self, clock_t startTime, bool isJumping, bool midJump);

Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("models/player.model");
    ent->think = player_think;
    ent->update = player_update;
    ent->damage = entity_damage;
    ent->onDeath = player_death;
    ent->isPlayer = 1;
    vector3d_copy(ent->position,position);
    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.h = 10.0;
    ent->bounds.w = 5.0;
    ent->bounds.d = 5.0;
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    //ent->hidden = 1;

    ent->isJumping = 0;
    ent->isDescending = 0;

    ent->health = 100;
    ent->attackType = 0;
    i=0;
    return ent;

    //entity_manager.entity_list = gfc_allocate_array(sizeof(Entity), 512);
}


void player_think(Entity *self)
{
    int temp = i;
    //check if monster1 is dead, if so set it back to NULL
    if(monster1){
        if(monster1->isDead == 1){
            monster1 = NULL;
        }
    }
    //set planes and boxes to test collision
    Plane3D bottomPlane = gfc_plane3d(0,0,-25,25);
    Plane3D topPlane = gfc_plane3d(0,0,10,10);
    Plane3D xPosPlane = gfc_plane3d(45,0,0,45);
    Plane3D xNegPlane = gfc_plane3d(-45,0,0,45);
    Plane3D yPosPlane = gfc_plane3d(0,45,0,45);
    Plane3D yNegPlane = gfc_plane3d(0,-45,0,45);
    Box centerBox = gfc_box(0,0,-25, 10,10,10);

    Vector3D forward = {0};
    Vector3D mForward = {0};
    Vector3D right = {0};
    Vector3D mRight = {0};
    Vector3D sprintForward = {0};
    //Vector3D sprintRight = {0};
    Vector2D w,mouse, mW;
    int mx,my;
    SDL_GetRelativeMouseState(&mx,&my);
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
    //const Uint8 *mouseButton;
    SDL_Event testEvent;
    
    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = w.x;
    right.y = w.y;

    //check for goo effects, eventually make them work on entities
    if(fastGoo != NULL || slowGoo != NULL){
        for(int j=0; j<i; j++){
            if(fastGoo && gfc_box_overlap(entityList[j]->bounds, fastGoo->bounds)){
                Vector3D temp = {0};
                temp.x = 3;
                temp.y = 3;

                mW = vector2d_from_angle(entityList[j]->rotation.z);
                mForward.x = mW.x;
                mForward.y = mW.y;
                mForward = vector3d_multiply(mForward, temp); 
                mW = vector2d_from_angle(entityList[j]->rotation.z - GFC_HALF_PI);
                mRight.x = mW.x;
                mRight.y = mW.y;
                mRight = vector3d_multiply(mRight, temp);    
            }
            else if(slowGoo && gfc_box_overlap(entityList[j]->bounds, slowGoo->bounds)){
                Vector3D temp = {0};
                temp.x = 0.5;
                temp.y = 0.5;

                mW = vector2d_from_angle(entityList[j]->rotation.z);
                mForward.x = mW.x;
                mForward.y = mW.y;
                mForward = vector3d_multiply(mForward, temp); 
                mW = vector2d_from_angle(entityList[j]->rotation.z - GFC_HALF_PI);
                mRight.x = mW.x;
                mRight.y = mW.y;
                mRight = vector3d_multiply(mRight, temp); 
            }
            if(fastGoo && gfc_box_overlap(self->bounds, fastGoo->bounds)){
                Vector3D temp = {0};
                temp.x = 3;
                temp.y = 3;

                w = vector2d_from_angle(self->rotation.z);
                forward.x = w.x;
                forward.y = w.y;
                forward = vector3d_multiply(forward, temp); 
                w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
                right.x = w.x;
                right.y = w.y;
                right = vector3d_multiply(right, temp);  
            }
            else if(slowGoo && gfc_box_overlap(self->bounds, slowGoo->bounds)){
                Vector3D temp = {0};
                temp.x = 0.5;
                temp.y = 0.5;

                w = vector2d_from_angle(self->rotation.z);
                forward.x = w.x;
                forward.y = w.y;
                forward = vector3d_multiply(forward, temp); 
                w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
                right.x = w.x;
                right.y = w.y;
                right = vector3d_multiply(right, temp);
            }
            else{
                mW = vector2d_from_angle(entityList[j]->rotation.z);
                mForward.x = mW.x;
                mForward.y = mW.y;

                w = vector2d_from_angle(entityList[j]->rotation.z - GFC_HALF_PI);
                mRight.x = mW.x;
                mRight.y = mW.y;
            }
        }
    }
    

    //Entity *monster1;

    //for checking for lag
    // clock_t startTime = clock();
    // bool isJumping = false;
    // bool midJump = false;

    //switch attacks
    if(keys[SDL_SCANCODE_TAB]){
        //slog("tab pressed");
        int attack = self->attackType;
        //slog("attack: %i", attack);
        if(attack + 1 >= 5){
            self->attackType = 0;
        }
        else{
            self->attackType += 1;
        }
    }
    //movement
    if (keys[SDL_SCANCODE_W])
    {   
        Vector3D p = self->position;
        if(p.x>=0){//x is pos
            if(p.y>=0){//y is pos
                if(collision_box_to_plane_x_pos(self->bounds, xPosPlane) && collision_box_to_plane_y(self->bounds, yPosPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, forward);
                }
                else{
                    self->position.x-=1;
                    self->position.y-=1;
                }
            }
            else{//y is neg
                if(collision_box_to_plane_x_pos(self->bounds, xPosPlane) && collision_box_to_plane_y(self->bounds, yNegPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                        vector3d_add(self->position, self->position, forward);
                }
                else{
                    self->position.x-=1;
                    self->position.y+=1;
                }
            }
        }
        else{//x is neg
            if(p.y>=0){//y is pos
                if(collision_box_to_plane_x_neg(self->bounds, xNegPlane) && collision_box_to_plane_y(self->bounds, yPosPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, forward);
                }
                else{
                    self->position.x+=1;
                    self->position.y-=1;
                }
            }
            else{//y is neg
                if(collision_box_to_plane_x_neg(self->bounds, xNegPlane) && collision_box_to_plane_y(self->bounds, yNegPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, forward);
                }
                else{
                    self->position.x+=1;
                    self->position.y+=1;
                }
            }
        } 
        //vector3d_add(self->position, self->position, forward);
    }
    if (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_LSHIFT]){ //sprint
        Vector3D temp = {0};
        temp.x = 2;
        temp.y = 2;

        sprintForward = vector3d_multiply(forward, temp);
        vector3d_add(self->position, self->position, sprintForward);
        //vector3d_add(self->velocity, self->velocity, forward);
    }
    if (keys[SDL_SCANCODE_S])
    {
        Vector3D p = self->position;
        if(p.x>=0){//x is pos
            if(p.y>=0){//y is pos
                if(collision_box_to_plane_x_pos(self->bounds, xPosPlane) && collision_box_to_plane_y(self->bounds, yPosPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, -forward);
                }
                else{
                    self->position.x-=1;
                    self->position.y-=1;
                }
            }
            else{//y is neg
                if(collision_box_to_plane_x_pos(self->bounds, xPosPlane) && collision_box_to_plane_y(self->bounds, yNegPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                        vector3d_add(self->position, self->position, -forward);
                }
                else{
                    self->position.x-=1;
                    self->position.y+=1;
                }
            }
        }
        else{//x is neg
            if(p.y>=0){//y is pos
                if(collision_box_to_plane_x_neg(self->bounds, xNegPlane) && collision_box_to_plane_y(self->bounds, yPosPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, -forward);
                }
                else{
                    self->position.x+=1;
                    self->position.y-=1;
                }
            }
            else{//y is neg
                if(collision_box_to_plane_x_neg(self->bounds, xNegPlane) && collision_box_to_plane_y(self->bounds, yNegPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, -forward);
                }
                else{
                    self->position.x+=1;
                    self->position.y+=1;
                }
            }
        } 
        //vector3d_add(self->position,self->position,-forward);        
    }
    // if (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_LSHIFT]){
        
    //     vector3d_add(self->position, self->position, -sprintForward);
    //     //vector3d_add(self->velocity, self->velocity, -forward);
    // }
    if (keys[SDL_SCANCODE_D])
    {
        Vector3D p = self->position;
        if(p.x>=0){//x is pos
            if(p.y>=0){//y is pos
                if(collision_box_to_plane_x_pos(self->bounds, xPosPlane) && collision_box_to_plane_y(self->bounds, yPosPlane)){
                    vector3d_add(self->position, self->position, right);
                }
                else{
                    self->position.x-=1;
                    self->position.y-=1;
                }
            }
            else{//y is neg
                if(collision_box_to_plane_x_pos(self->bounds, xPosPlane) && collision_box_to_plane_y(self->bounds, yNegPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                        vector3d_add(self->position, self->position, right);
                }
                else{
                    self->position.x-=1;
                    self->position.y+=1;
                }
            }
        }
        else{//x is neg
            if(p.y>=0){//y is pos
                if(collision_box_to_plane_x_neg(self->bounds, xNegPlane) && collision_box_to_plane_y(self->bounds, yPosPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, right);
                }
                else{
                    self->position.x+=1;
                    self->position.y-=1;
                }
            }
            else{//y is neg
                if(collision_box_to_plane_x_neg(self->bounds, xNegPlane) && collision_box_to_plane_y(self->bounds, yNegPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, right);
                }
                else{
                    self->position.x+=1;
                    self->position.y+=1;
                }
            }
        } 
        //vector3d_add(self->position,self->position,right);
    }
    // if (keys[SDL_SCANCODE_D] && keys[SDL_SCANCODE_LSHIFT]){
    //     vector3d_add(self->position, self->position, sprintRight);
    //     //vector3d_add(self->velocity, self->velocity, right);
    // }
    if (keys[SDL_SCANCODE_A])    
    {
        Vector3D p = self->position;
        if(p.x>=0){//x is pos
            if(p.y>=0){//y is pos
                if(collision_box_to_plane_x_pos(self->bounds, xPosPlane) && collision_box_to_plane_y(self->bounds, yPosPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, -right);
                }
                else{
                    self->position.x-=1;
                    self->position.y-=1;
                }
            }
            else{//y is neg
                if(collision_box_to_plane_x_pos(self->bounds, xPosPlane) && collision_box_to_plane_y(self->bounds, yNegPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                        vector3d_add(self->position, self->position, -right);
                }
                else{
                    self->position.x-=1;
                    self->position.y+=1;
                }
            }
        }
        else{//x is neg
            if(p.y>=0){//y is pos
                if(collision_box_to_plane_x_neg(self->bounds, xNegPlane) && collision_box_to_plane_y(self->bounds, yPosPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, -right);
                }
                else{
                    self->position.x+=1;
                    self->position.y-=1;
                }
            }
            else{//y is neg
                if(collision_box_to_plane_x_neg(self->bounds, xNegPlane) && collision_box_to_plane_y(self->bounds, yNegPlane) && !gfc_box_overlap(self->bounds, centerBox)){
                    vector3d_add(self->position, self->position, -right);
                }
                else{
                    self->position.x+=1;
                    self->position.y+=1;
                }
            }
        } 
        //vector3d_add(self->position,self->position,-right);
    }
    // if (keys[SDL_SCANCODE_A] && keys[SDL_SCANCODE_LSHIFT]){
    //     vector3d_add(self->position, self->position, -sprintRight);
    //     //vector3d_add(self->velocity, self->velocity, -right);
    // }
    /*
    if (keys[SDL_SCANCODE_SPACE]){
        isJumping = true;
        midJump = false;
        clock_t startTime = clock();
        bool startJump = jump(self, startTime, isJumping, midJump);
        
        //starts the jump

        

    }
    if(isJumping){
        isJumping = jump(self, startTime, isJumping, midJump);
    }
    */
    if(!self->isJumping && !self->isDescending){
        if(keys[SDL_SCANCODE_SPACE]){
            slog("space pressed");
            self->startPosition = self->position.z;
            self->isJumping = 1;
        }
    }
    //if (keys[SDL_SCANCODE_SPACE])self->position.z += 1;
    if (keys[SDL_SCANCODE_X]){//go up
        if(collision_box_to_plane_z_up(self->bounds, topPlane) && !gfc_box_overlap(self->bounds, centerBox)){
            self->position.z+=1;
        }
        else{
            self->position.z-=1;
        }
        //self->position.z += 1;
    }
    if (keys[SDL_SCANCODE_Z]){//go down
        if(collision_box_to_plane_z_down(self->bounds, bottomPlane) && !gfc_box_overlap(self->bounds, centerBox)){
            self->position.z-=1;
        }
        else{
            self->position.z+=1;
        }
        //self->position.z -= 1;
    }
    
    if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050;
    if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050;
    if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050;
    
    if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);

    if (keys[SDL_SCANCODE_F3])
    {
        thirdPersonMode = !thirdPersonMode;
        self->hidden = !self->hidden;
    }
    if(keys[SDL_SCANCODE_C]){
        slog("x: %f, y: %f, z: %f\n", self->rotation.x, self->rotation.y, self->rotation.z);
    }
    //keys for spawning
    if (keys[SDL_SCANCODE_1] && !keys[SDL_SCANCODE_LSHIFT]){
        //printf("x: %f, y: %f, z: %f\n", self->position.x, self->position.y, self->position.z);
        //printf("\n");
        monster1 = monster1_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster1){
            //monster1->selected = 1;
            //entity_manager.entity_list[i] = monster1;
            entityList[i] = monster1;
            //slog("added monster to entity list");
            i++;
        }
    }
    //when pressing LSHIFT and the number it will make that entity take damage
    if(keys[SDL_SCANCODE_1] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster1){
            monster1->damage(self, monster1, self->attackDamage, 0);
            if(monster1->health == 0){
                monster1->isDead = 1;
                monster1 = NULL;
            }
        }
    }
    if (keys[SDL_SCANCODE_2] && !keys[SDL_SCANCODE_LSHIFT]){
        monster2 = monster2_kong_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster2){
            //monster1->selected = 1;
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster2;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_2] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster2){
            monster2->damage(self, monster2, self->attackDamage, 0);
            if(monster2->health == 0){
                monster2->isDead = 1;
                monster2 = NULL;
            }
        }
    }
    if (keys[SDL_SCANCODE_3] && !keys[SDL_SCANCODE_LSHIFT]){
        monster3 = monster3_porygon_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster3){
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster3;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_3] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster3){
            monster3->damage(self, monster3, self->attackDamage, 0);
            if(monster3->health == 0){
                monster3->isDead = 1;
                monster3 = NULL;
            }
        }
    }
    if (keys[SDL_SCANCODE_4] && !keys[SDL_SCANCODE_LSHIFT]){
        monster4 = monster4_skelly_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster4){
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster4;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_4] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster4){
            monster4->damage(self, monster4, self->attackDamage, 0);
            if(monster4->health == 0){
                monster4->isDead = 1;
                monster4 = NULL;
            }
        }
    }
    if (keys[SDL_SCANCODE_5] && !keys[SDL_SCANCODE_LSHIFT]){
        monster5 = monster5_mario_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster5){
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster5;
            //slog("added monster to entity list");
            i++;
        }
    } //monster1->onDeath(monster1);
    if(keys[SDL_SCANCODE_5] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster5){
            monster5->damage(self, monster5, self->attackDamage, 0);
            if(monster5->health == 0){
                monster5->isDead = 1;
                monster5 = NULL;
            }
        }
    }
    if(keys[SDL_SCANCODE_6] && !keys[SDL_SCANCODE_LSHIFT]){
        monster6 = monster6_yoshi_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster6){
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster6;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_6] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster6){
            //self->attackType = 1;
            monster6->damage(self, monster6, self->attackDamage, 0);
            if(monster6->health == 0){
                monster6->isDead = 1;
                monster6 = NULL;
            }
        }
        else
            return;
    }
    if(keys[SDL_SCANCODE_7] && !keys[SDL_SCANCODE_LSHIFT]){
        monster7 = monster7_creeper_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster7){
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster7;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_7] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster7){
            //self->attackType = 2;
            monster7->damage(self, monster7, self->attackDamage, 0);
            if(monster7->health == 0){
                monster7->isDead = 1;
                monster7 = NULL;
            }
        }
    }
    if(keys[SDL_SCANCODE_8] && !keys[SDL_SCANCODE_LSHIFT]){
        monster8 = monster8_finn_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster8){
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster8;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_8] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster8){
            //self->attackType = 4;
            monster8->damage(self, monster8, self->attackDamage, 0);
            if(monster8->health == 0){
                monster8->isDead = 1;
                monster8 = NULL;
            }
        }
    }
    if(keys[SDL_SCANCODE_9] && !keys[SDL_SCANCODE_LSHIFT]){
        monster9 = monster9_goomba_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster9){
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster9;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_9] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster9){
            //self->attackType = 3;
            monster9->damage(self, monster9, self->attackDamage, 0);
            if(monster9->health == 0){
                monster9->isDead = 1;
                monster9 = NULL;
            }
        }
    }
    if(keys[SDL_SCANCODE_0] && !keys[SDL_SCANCODE_LSHIFT]){
        monster10 = monster10_arlo_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster10){
            //entity_manager.entity_list[i] = *monster1;
            entityList[i] = monster10;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_0] && keys[SDL_SCANCODE_LSHIFT]){
        if(monster10){
            //self->attackType = 0;
            monster10->damage(self, monster10, self->attackDamage, 0);
            if(monster10->health == 0){
                monster10->isDead = 1;
                monster10 = NULL;
            }
        }
    }

    if(keys[SDL_SCANCODE_E] && !keys[SDL_SCANCODE_LALT]){
        fence = defense1_smallFence_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
        fence->state = ES_BASE;
        if(fence){
            entityList[i] = fence;
            //slog("added monster to entity list");
            i++;
        }
        //defenseList[defenseCount] = *fence;
        //self->defenseBounds[0] = gfc_box(1,1,1,1,1,1);
        //slog("%d", self->defenseBounds[0]);
        self->defenseBounds[defenseCount] = fence->bounds;
        defenseCount++;
        self->defenseCount = defenseCount;
        
        //self->defenseList = defenseList;
    }
    if(keys[SDL_SCANCODE_E] && keys[SDL_SCANCODE_LALT]){
        if(fence){
            fence->state += 1;
            if(fence->state >= 8){
                fence->state = 4;
            }
            fence->stateSwitched = 1;
        }
    }
    if(keys[SDL_SCANCODE_R] && !keys[SDL_SCANCODE_LALT]){
        wall = defense2_smallWall_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
        //defenseList[defenseCount] = *wall;
        if(wall){
            entityList[i] = wall;
            //slog("added monster to entity list");
            i++;
        }
        self->defenseBounds[defenseCount] = wall->bounds;
        self->defenseCount = defenseCount;
        defenseCount++;
        //self->defenseList = defenseList;
    }
    if(keys[SDL_SCANCODE_R] && keys[SDL_SCANCODE_LALT]){
        if(wall){
            wall->state += 1;
            if(wall->state >= 8){
                wall->state = 4;
            }
            wall->stateSwitched = 1;
        }
    }
    if(keys[SDL_SCANCODE_T] && !keys[SDL_SCANCODE_LALT]){
        turret1 = defense3_turret1_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
        //defenseList[defenseCount] = *turret1;
        if(turret1){
            entityList[i] = turret1;
            //slog("added monster to entity list");
            i++;
        }
        self->defenseBounds[defenseCount] = turret1->bounds;
        self->defenseCount = defenseCount;
        defenseCount++;
        //self->defenseList = defenseList;
    }
    if(keys[SDL_SCANCODE_T] && keys[SDL_SCANCODE_LALT]){
        if(turret1){
            turret1->state += 1;
            if(turret1->state >= 8){
                turret1->state = 4;
            }
            turret1->stateSwitched = 1;
        }
    }
    if(keys[SDL_SCANCODE_Y] && !keys[SDL_SCANCODE_LALT]){
        turret2 =defense4_turret2_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
        //defenseList[defenseCount] = *turret2;
        if(turret2){
            entityList[i] = turret2;
            //slog("added monster to entity list");
            i++;
        }
        self->defenseBounds[defenseCount] = turret2->bounds;
        self->defenseCount = defenseCount;
        defenseCount++;
        //self->defenseList = defenseList;
    }
    if(keys[SDL_SCANCODE_Y] && keys[SDL_SCANCODE_LALT]){
        if(turret2){
            turret2->state += 1;
            if(turret2->state >= 8){
                turret2->state = 4;
            }
            turret2->stateSwitched = 1;
        }
    }
    if(keys[SDL_SCANCODE_U] && !keys[SDL_SCANCODE_LALT]){
        turret3 = defense5_turret3_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
        //defenseList[defenseCount] = *turret3;
        if(turret3){
            entityList[i] = turret3;
            //slog("added monster to entity list");
            i++;
        }
        self->defenseBounds[defenseCount] = turret3->bounds;
        self->defenseCount = defenseCount;
        defenseCount++;
    }
    if(keys[SDL_SCANCODE_U] && keys[SDL_SCANCODE_LALT]){
        if(turret3){
            turret3->state += 1;
            if(turret3->state >= 8){
                turret3->state = 4;
            }
            turret3->stateSwitched = 1;
        }
    }
    while(SDL_PollEvent(&testEvent) > 0){
        slog("in SDL_PollEvent");
        //slog("%s", testEvent.type);
        switch (testEvent.type){
            case SDL_MOUSEBUTTONDOWN:
                if(testEvent.button.button == SDL_BUTTON_LEFT)
                    slog("button down");
                break;
            case SDL_MOUSEBUTTONUP:
                slog("button up");
                break;
        }
    }

    // int lClick = 0;
    // if(SDL_BUTTON(2)){
    //     if (lClick == 0){
    //         slog("mouse button pressed");
    //         lClick = 1;
    //     }
    // }
    //spawns for goo
    if(keys[SDL_SCANCODE_V]){
        slowGoo = slowGoo_new(self->position);
        if(slowGoo){
            entityList[i] = slowGoo;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_B]){
        fastGoo = fastGoo_new(self->position);
        if(fastGoo){
            entityList[i] = fastGoo;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_N]){
        dmgGoo = dmgGoo_new(self->position);
        if(dmgGoo){
            entityList[i] = dmgGoo;
            //slog("added monster to entity list");
            if(temp == i){
                i++;
            }
        }
    }
    if(keys[SDL_SCANCODE_M]){
        healGoo = healGoo_new(self->position);
        if(healGoo){
            entityList[i] = healGoo;
            //slog("added monster to entity list");
            i++;
        }
    }
    if(keys[SDL_SCANCODE_COMMA]){
        incDmgGoo = incDmgGoo_new(self->position);
        if(incDmgGoo){
            entityList[i] = incDmgGoo;
            //slog("added monster to entity list");
            i++;
        }
    }

    //save game
    if(keys[SDL_SCANCODE_Q] && !keys[SDL_SCANCODE_LSHIFT]){//test save
        //test json stuff
        SJson *body = sj_object_new();
        SJson *playerEnt = sj_object_new();
        playerSave(self, playerEnt);
        sj_object_insert(body, "playerInfo", playerEnt);
        for(int j=0; j<i; j++){ //loop through entity list, entityList[j]
            SJson *entity = sj_object_new();
            slog("pre save rotation x: %f, y: %f, z: %f", entityList[j]->rotation.x, entityList[j]->rotation.y, entityList[j]->rotation.z);
            // sj_object_insert(entity, "entityType", sj_new_int(entityList[j]->type));
            // sj_object_insert(entity, "entityNum", sj_new_int(entityList[j]->entityNum));
            // sj_object_insert(entity, "isPLayer", sj_new_int(entityList[j]->isPlayer));
            
            // sj_object_insert(entity, "position_x", sj_new_float(entityList[j]->position.x));
            // sj_object_insert(entity, "position_y", sj_new_float(entityList[j]->position.y));
            // sj_object_insert(entity, "position_z", sj_new_float(entityList[j]->position.z));
            
            // sj_object_insert(entity, "rotation_x", sj_new_float(entityList[j]->rotation.x));
            // sj_object_insert(entity, "rotation_y", sj_new_float(entityList[j]->rotation.y));
            // sj_object_insert(entity, "rotation_z", sj_new_float(entityList[j]->rotation.z));
            
            entity = testSave(entityList[j], entity);

            char str[20];
            sprintf(str, "Entity %i", j); //entities are numbered in the order that they are initialy spawned in
            sj_object_insert(body, str, entity);
        }
        //sj_echo(body);
        sj_save(body, "saves/entityList.json");
        slog("saved");
    }
    if(keys[SDL_SCANCODE_Q] && keys[SDL_SCANCODE_LSHIFT]){//test load
        slog("start load");
        SJson *body = sj_object_new();
        body = sj_load("saves/entityList.json");

        SJList *keysList = sj_object_get_keys_list(body);

        //load player info
        SJson *playerEnt = sj_object_new();
        playerEnt = sj_object_get_value(body, sj_list_get_nth(keysList, 0));
        playerLoad(self, playerEnt);

        slog("keyList size: %i, count: %i",keysList->size, keysList->count);
        //entityList = NULL; //reset entityList

        //reset entityList to 0, only needed when loading while already playing
        for(int j=0; j<512; j++){
            entityList[j] == NULL;
        }
        i=0;
        
        for(int j=1; j<keysList->count; j++){
            SJson *entity = sj_object_new();
            entity = sj_object_get_value(body, sj_list_get_nth(keysList, j));
            Entity *temp = entity_new();
            // int *type = NULL, *num = NULL, *isPlayer = NULL;
            // float *position_x = NULL, *position_y = NULL, *position_z = NULL, *rotation_x = NULL, *rotation_y = NULL, *rotation_z = NULL;
            sj_get_integer_value(sj_object_get_value(entity, "entityType"), &temp->type);
            sj_get_integer_value(sj_object_get_value(entity, "entityNum"), &temp->entityNum);
            // sj_get_integer_value(sj_object_get_value(entity, "isPlayer"), &temp->isPlayer);

            sj_get_float_value(sj_object_get_value(entity, "position_x"), &temp->position.x);
            sj_get_float_value(sj_object_get_value(entity, "position_y"), &temp->position.y);
            sj_get_float_value(sj_object_get_value(entity, "position_z"), &temp->position.z);

            sj_get_float_value(sj_object_get_value(entity, "rotation_x"), &temp->rotation.x);
            sj_get_float_value(sj_object_get_value(entity, "rotation_y"), &temp->rotation.y);
            sj_get_float_value(sj_object_get_value(entity, "rotation_z"), &temp->rotation.z);

            // /slog("Type: %i, Num: %i", type, num);

            // Vector3D position = vector3d(position_x, position_y, position_z);
            // Vector3D rotation = vector3d(rotation_x, rotation_y, rotation_z);
            // slog("position: x: %f, y:%f, z:%f", *position_x, *position_y, *position_z);
            // slog("rotation: x: %f, y:%f, z:%f", *rotation_x, *rotation_y, *rotation_z);

            Entity *newEnt;
            if(temp->isPlayer == 1){
                //is the player
            }
            else if(temp->type == 0){
                //is a monster
                switch(temp->entityNum){
                    case 1:
                        newEnt = NULL;
                        newEnt = monster1_new(temp->position);
                        slog("spawning monster1, EntityType: %i", temp->type);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 2:
                        newEnt = NULL;
                        newEnt = monster2_kong_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 3:
                        newEnt = NULL;
                        newEnt = monster3_porygon_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 4:
                        newEnt = NULL;
                        newEnt = monster4_skelly_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 5:
                        newEnt = NULL;
                        newEnt = monster5_mario_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 6:
                        newEnt = NULL;
                        newEnt = monster6_yoshi_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 7:
                        newEnt = NULL;
                        newEnt = monster7_creeper_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 8:
                        newEnt = NULL;
                        newEnt = monster8_finn_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 9:
                        newEnt = NULL;
                        newEnt = monster9_goomba_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 10:
                        newEnt = NULL;
                        newEnt = monster10_arlo_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                }
            }
            else if(temp->type == 1){
                //is a wall
                switch(temp->entityNum){
                    case 1:
                        newEnt = entity_new();
                        temp->rotation.z += (M_PI/2.0);
                        newEnt = defense1_smallFence_new(temp->position, temp->rotation);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 2:
                        newEnt = NULL;
                        temp->rotation.z += (M_PI/2.0);
                        newEnt = defense2_smallWall_new(temp->position, temp->rotation);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                }
            }
            else if(temp->type == 2){
                //is a turret
                switch(temp->entityNum){
                    case 1:
                        newEnt = NULL;
                        temp->rotation.z += (M_PI/2.0);
                        newEnt = defense3_turret1_new(temp->position, temp->rotation);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 2:
                        newEnt = NULL;
                        temp->rotation.z += (M_PI/2.0);
                        newEnt = defense4_turret2_new(temp->position, temp->rotation);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 3:
                        newEnt = NULL;
                        temp->rotation.z += (M_PI/2.0);
                        newEnt = defense5_turret3_new(temp->position, temp->rotation);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                }
            }
            else if(temp->type == 3){
                //is goo
                switch(temp->entityNum){
                    case 1:
                        newEnt = NULL;
                        newEnt = slowGoo_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 2:
                        newEnt = NULL;
                        newEnt = fastGoo_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 3:
                        newEnt = NULL;
                        newEnt = dmgGoo_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 4:
                        newEnt = NULL;
                        newEnt = healGoo_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                    case 5:
                        newEnt = NULL;
                        newEnt = incDmgGoo_new(temp->position);
                        newEnt = testLoad(newEnt, entity);
                        entityList[j] = newEnt;
                        i++;
                        break;
                }
            }
        }

        slog("load done");
    }
    if(keys[SDL_SCANCODE_O]){
        for(int j=0; j<i; j++){
            slog("entityList[j]->type: %i", entityList[j]->type);
        }
    }
    if(keys[SDL_SCANCODE_P]){
        SJson *entList = sj_object_new();
        saveGame_to_json(self, "saves/playerSave.json");
        //testSave(self, "saves/saveTest.json");
        for(int j=0; j<i; j++){
            if(saveGame_to_json(entityList[j], "saves/saveTest.json") == 0)
                slog("save failed");
            else
                slog("saved");
        }
        for(int j=0; j<i; j++){
            sj_object_insert(entList, "entityType", sj_new_int(entityList[j]->type));
            sj_object_insert(entList, "entityNum", sj_new_int(entityList[j]->entityNum));
            sj_echo(entList);
            sj_save(entList, "saves/entityList.json");
        }
        
        //sj_free(entList);
    }
    //load json
    if(keys[SDL_SCANCODE_L]){
        loadGame_from_json(self, "saves/playerSave.json");
        for(int j=0; j<i; j++){
            if(loadGame_from_json(entityList[j], "saves/saveTest.json") == 0)
                slog("load failed");
            else{
                //monster1_new(monster1->position);
                slog("loaded");
            }
        }
    }

    //test entity list
    if(keys[SDL_SCANCODE_RCTRL]){
        for(int j=1; j<i; j++){
            slog("entityType: %i", entityList[j]->type);
            slog("entityNum: %i", entityList[j]->entityNum);
        }
    }
    
}

void player_update(Entity *self, Entity *player)//needed player twice to stop a warning
{
    //updating all entites here instead of game.c so I can pass the defenseList to the enemies for collision
    //entity_update_all(player, defenseList, defenseCount);
    //update bounds
    self->bounds.x = self->position.x;
    self->bounds.y = self->position.y;
    self->bounds.z = self->position.z;

    Vector3D forward = {0};
    Vector3D position;
    Vector3D rotation;
    Vector2D w;

    //set bounding for center box and ground
    Box centerBox = gfc_box(0,0,-25, 10,10,10);
    Plane3D bottomPlane = gfc_plane3d(0,0,-25,25);
    
    if (!self)return;
    
    vector3d_copy(position,self->position);
    vector3d_copy(rotation,self->rotation);
    if (thirdPersonMode)
    {
        position.z += 100;
        rotation.x += M_PI*0.125;
        w = vector2d_from_angle(self->rotation.z);
        forward.x = w.x * 100;
        forward.y = w.y * 100;
        vector3d_add(position,position,-forward);
    }
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(rotation);

    if(self->isJumping){
        //slog("in jumping");
        if(self->position.z - self->startPosition <= 10){//jump height
            self->position.z += 1.5;
        }
        else{
            self->isJumping = 0;
            self->isDescending = 1;
        }
    }
    else if(self->isDescending){
        //slog("in descending");
        //slog("%f", self->startPosition - self->position.z);
        if(self->position.z - self->startPosition >= 0){
            //slog("descending");
            if(!gfc_box_overlap(self->bounds, centerBox) && collision_box_to_plane_z_down(self->bounds, bottomPlane)){
                self->position.z -= 1.5;
            }
            else{
                self->isDescending = 0;
                //slog("collision with center box");
                self->position.z += 1;
            }
        }
        else{
            //slog("stop descending");
            self->isDescending = 0;
        }
    }
    //clock_t currentTime = clock();
    //self->currentTime=currentTime;
    //self->points+=1;
    //points = self->points;

    //check if dead
    if(self->health == 0){
        self->onDeath(self);
    }

    /*
    update damage based on attack
    R_bullet = 15 damage
    R_fire = 30 damage
    R_ice = 25 damage
    R_magic = 35 damage
    R_melee = 10 damage
    */
   switch(self->attackType){
        case R_bullet:
            self->attackDamage = 15;
            break;
        case R_fire:
            self->attackDamage = 25;
            break;
        case R_ice:
            self->attackDamage = 25;
            break;
        case R_magic:
            self->attackDamage = 30;
            break;
        case R_melee:
            self->attackDamage = 10;
            break;
   }
    
    if(collision_box_to_plane_z_down(self->bounds, bottomPlane)){
        if(!gfc_box_overlap(self->bounds, centerBox)){
            if(!(self->isJumping || self->isDescending)){
                self->position.z -= 1;
            }
        }
        
    }

    //check for goo effects
    if(dmgGoo){
        //slog("dmgGoo exists");
        for(int j=0; j<i; j++){
            //slog("j: %d, loop: %d", j, loopNum);
            //slog("able to get in for");
            if(entityList[j]->type == ET_goo){
                continue;
            }
            if(gfc_box_overlap(entityList[j]->bounds, dmgGoo->bounds)){
                //slog("trying to damage %i", j);
                entity_damage(dmgGoo, entityList[j], 1, 0);
                //slog("trying to damage %i", entityList[j]->health);
            }
        }
        if(gfc_box_overlap(self->bounds, dmgGoo->bounds)){
            entity_damage(dmgGoo, self, 1, 0);
        }
    }
    if(healGoo){
        if(gfc_box_overlap(self->bounds, healGoo->bounds)){
            entity_damage(healGoo, self, 1, 1);
        }
    }
    if(incDmgGoo){
        if(gfc_box_overlap(self->bounds, incDmgGoo->bounds)){
            self->attackDamage *= 2;
        }
    }


    //pass entity list into player entity so monsters can see it
    // for(int j=0; j<i; j++){
    //     switch(entityList[j]->type){
    //         case ET_monster:
    //     }

    // }
}

void player_damage(int damage, Entity *self, int heal, Entity *inflictor){
    if(heal == 0){//damage not heal
        int temp = self->health - damage;
        if(temp<0) temp = 0;
        self->health = 0;
    }
    else{//heal player with number from damage value
        int temp = self->health + damage;
        if(temp>100) temp = 100;
        self->health = temp;
    }
}

void player_death(Entity *self){
    //remove player entity
    //show you died text like dark souls
    //respawn player or send to main menu
}

/*eol@eof*/
