#include "simple_logger.h"
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
#include "collision.h"

#include "gfc_primitives.h"
//#include "gfc_vector.h"

#include <stdbool.h>
#include <time.h>
#include <inttypes.h>

static int thirdPersonMode = 0;
int points = 0;
Entity *monster1;
void player_think(Entity *self);
void player_update(Entity *self);
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
    ent->damage = player_damage;
    ent->onDeath = player_death;
    vector3d_copy(ent->position,position);
    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.h = 10.0;
    ent->bounds.w = 10.0;
    ent->bounds.d = 10.0;
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    //ent->hidden = 1;

    ent->isJumping = 0;
    ent->isDescending = 0;

    ent->health = 100;
    return ent;
}


void player_think(Entity *self)
{
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
    Vector3D right = {0};
    Vector3D sprintForward = {0};
    //Vector3D sprintRight = {0};
    Vector2D w,mouse;
    int mx,my;
    SDL_GetRelativeMouseState(&mx,&my);
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = w.x;
    right.y = w.y;

    //Entity *monster1;

    // clock_t startTime = clock();
    // bool isJumping = false;
    // bool midJump = false;

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

    if (keys[SDL_SCANCODE_1]){
        printf("x: %f, y: %f, z: %f\n", self->position.x, self->position.y, self->position.z);
        //printf("\n");
        monster1 = monster10_arlo_new(vector3d(self->position.x, self->position.y, self->position.z));
        if(monster1) monster1->selected = 1;
    }
    if (keys[SDL_SCANCODE_2]){
        if(monster1)slog("%i", monster1->health);
        else{
            slog("monster1 pointer not set");
        }
        //printf("points: %i", self->points);
        //printf("\n");
    }
    if (keys[SDL_SCANCODE_3]){
        //slog("player position: x: %f, y: %f, z: %f", self->position.x, self->position.y, self->position.z);
        if(monster1){
            monster1->damage(10, monster1, 0, self);
            if(monster1->health == 0){
                monster1->isDead = 1;
            }
        }
    }
    if (keys[SDL_SCANCODE_4]){
        self->points+=10;
    }
    if (keys[SDL_SCANCODE_5]){
        //monster1->health -= 10;
        //monster1_damage(10, monster1, 0, self);
        //monster1->damage(10, monster1, 0, self);
        if(monster1){
            monster1->onDeath(monster1);
            monster1=NULL;
        }
        else{
            slog("monster1 pointer not set");
        }
    } //monster1->onDeath(monster1);
    if(keys[SDL_SCANCODE_6]){
        monster2_kong_new(vector3d(self->position.x, self->position.y, self->position.z));
    }
}

void player_update(Entity *self)
{
    //update bounds
    self->bounds.x = self->position.x;
    self->bounds.y = self->position.y;
    self->bounds.z = self->position.z;

    Vector3D forward = {0};
    Vector3D position;
    Vector3D rotation;
    Vector2D w;

    //set bounding for center box
    Box centerBox = gfc_box(0,0,-25, 10,10,10);
    
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
        if(self->position.z - self->startPosition <= 7.5){//jump height
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
            if(!gfc_box_overlap(self->bounds, centerBox)){
                self->position.z -= 1.5;
            }
            else{
                self->isDescending = 0;
                slog("collision with center box");
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
    //repspawn player or send to main menu
}

// bool jump(Entity *self, clock_t startTime, bool isJumping, bool midJump){
//     do{
//         printf("elapsed time: %ld, currentTime: %d, startTime: %ld\n", self->currentTime - startTime, self->currentTime, startTime);
//         if(startTime - self->currentTime >= .50 && midJump == false){//go up
//             self->position.z += 5;
//             return midJump = true;
//         }
//         else{//go down
//             self->position.z -= 5;
//             isJumping = false;
//             return isJumping;
//         }
       
//     }
//     while(isJumping);
// }
/*eol@eof*/
