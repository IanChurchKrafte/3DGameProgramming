/*
Keep track of all spawned entities in a list
when checking if raycast hits a box just go through that list
this also helps incase multiple of same entity is spawned its not lost by setting the pointer again
*/

#include "simple_logger.h"
#include "simple_json.h"
#include "simple_json_object.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "hitBoxEnt.h"
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
#include "gfc_audio.h"
//#include "gfc_vector.h"

#include <stdbool.h>
#include <time.h>
#include <inttypes.h>

float edge3DLength(Edge3D e){
    float a,b,c;
    a = e.a.x - e.b.x;
    b = e.a.y - e.b.y;
    c = e.a.z - e.b.z;
    return vector3d_magnitude(vector3d(a,b,c));
}

//for wave spawning
time_t *lastTime = 0, *currentTime = 0;

int i=0, m=0; //i to keep track of how many entities, m for walls

Entity *entityList[512];
Entity *wallList[256];
static int thirdPersonMode = 0;
int points = 0, bossSpawning = 0, wallDestroyed = 0;
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
void loadEntity(Entity *self);
void player_spawnWave();
void rayCast(Entity *self, float distance);
Entity *creeper, *creeperBounds, *fenceBounds = NULL;
Sound *gunshot;

/*
will need to establish a 2d grid for the AI to be able to traverse on to be able to have pathfinding
The plan is to have the first AI that makes contact with the wall defense starts to attack it,
while that AI is attacking it the others will continue on the path that is built by the player,
they will continue on the path using pathfinding and A* search
*/


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
    ent->bounds.h = 5.0;
    ent->bounds.w = 5.0;
    ent->bounds.d = 5.0;
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    //ent->hidden = 1;

    ent->isJumping = 0;
    ent->isDescending = 0;

    ent->health = 150;
    ent->attackType = 0;
    i=0;

    //set the time to relative 0
    time(&lastTime);
    time(&currentTime);

    ent->editMode = 0;

    //testing for points
    ent->points = 15000;

    // creeper = monster7_creeper_new(vector3d(-32, 23, -24));
    // entityList[i] = creeper;
    // i++;
    // creeperBounds = hitBoxEnt_new(creeper->position, creeper);
    return ent;

    //entity_manager.entity_list = gfc_allocate_array(sizeof(Entity), 512);
}


void player_think(Entity *self)
{
    //try to spawn wave
    player_spawnWave(vector3d(-32, 23, -24), self);

    

    //trying to draw the hit box
    // for(int j=0; j<i; j++){
    //     Rect front, side1, side2, back;
    //     front.x = entityList[j]->bounds.x;
    //     front.y = entityList[j]->bounds.y;
    //     front.h = entityList[j]->bounds.h;
    //     front.w = entityList[j]->bounds.w;

    //     gf2d_draw_rect(front, gfc_color(0,1,0,1));
    // }
    int temp = i;

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

    if(keys[SDL_SCANCODE_MINUS]){//just to put a breakpoint in gdb to check values
        slog("TEST");

        slog("TEST");
    }
    //ray casting test
    
    if(keys[SDL_SCANCODE_LCTRL]){ //not current ray casting button, f2 is

        // slog("rayCast started");
        // Vector3D playerPoint = vector3d(self->position.x, self->position.y, self->position.y);
        // slog("PlayerPoint: x: %f, y: %f, z: %f", playerPoint.x, playerPoint.y, playerPoint.z);
        // //point far off in distance to draw an edge too, in the direction the player is looking
        // Vector3D testingPoint = vector3d(forward.x+1000, forward.y+1000, playerPoint.z);
        // slog("PlayerPoint: x: %f, y: %f, z: %f", testingPoint.x, testingPoint.y, testingPoint.z);
        // Edge3D rayCast;
        // rayCast.a = playerPoint;
        // rayCast.b = testingPoint;
        Vector3D *poc = NULL, *normal = NULL;
        // Shape drawShape;
        // drawShape.type = edge;
        // drawShape.s = 
        float distance = 10.0;
        float rot = self->rotation.z;
        slog("player rotation: %f, %f, %f", self->rotation.x, self->rotation.y, self->rotation.z);
        slog("player position: %f, %f, %f", self->position.x, self->position.y, self->position.z);
        Vector3D playerPoint = vector3d(self->position.x, self->position.y, self->position.z);
        float ang = rot;
        slog("ang: %f, sinf(ang): %f, cosf(ang): %f", ang, sinf(ang), cosf(ang));
        // // Vector3D *rayForward= NULL, *rayUp=NULL, *rayRight=NULL;
        // // vector3d_angle_vectors(self->rotation, rayForward, rayRight, rayUp);
        // // slog("rayForward: %f, %f, %f", rayForward->x, rayForward->y, rayForward->z);
        float c = cosf(ang);
        float s = sinf(ang);
        float x1,y1;
        if(c == 0.0){ //looking directly in the x axis
            slog('shouldnt be here');
            x1 = playerPoint.x + 1 * distance;
            y1 = playerPoint.y + 1 * distance;
        }
        else{
            x1 = playerPoint.x + sinf(ang) * distance;
            y1 = playerPoint.y + cosf(ang) * distance;
            slog("sinf(ang) * distance = %f", playerPoint.x + sinf(ang) * distance);
            slog("cosf(ang) * distance = %f", playerPoint.y + cosf(ang) * distance);
        }

        // Vector3D testPoint = self->position;
        // Vector3D temp = {0};
        // temp.x = 5;
        // temp.y = 5;
            // vector2d_move_towards();
            // self->position.x = selfPos->x;
            // self->position.y = selfPos->y;
        // vector3d_add(testPoint, playerPoint, distance);
        Vector3D testPoint = vector3d(x1, y1, playerPoint.z);
        //self->position = testPoint;
        //vector3d_add(testPoint, )
        slog("test point: %f, %f, %f", testPoint.x, testPoint.y, testPoint.z);
        Edge3D ray;
        ray.a = playerPoint;
        ray.b = testPoint;
        //Edge rayCast = gfc_edge(playerPoint.x, playerPoint.y, testPoint.x, testPoint.y);
        // Vector2D testPoint = vector2d(forward.x+1000, forward.y+1000);
        
        // Shape drawShape;
        // drawShape.type = ST_EDGE;
        // drawShape.s.e = rayCast;
        // vector2d_angle


        //gf2d_draw_shape(drawShape, gfc_color(0,1,0,1), vector2d(0,0));

        for(int j=0; j<i; j++){
            slog("in entity loop");
            if(gfc_edge_box_test(ray, entityList[j]->bounds, poc, normal)){
                slog("intersected with entity");
                // slog("poc: %f, %f, %f", poc->x, poc->y, poc->z);
                // slog("normal: %f, %f, %f", normal->x, normal->y, normal->z);
            }
        }
    }

    // hitBoxEnt_update(creeperBounds, creeper);
    // if(fence != NULL && fenceBounds != NULL)
    //     hitBoxEnt_update(fenceBounds, fence);
    if(keys[SDL_SCANCODE_F2]){ //raycast, distance based off attack type, probably should change the button
        gunshot = gfc_sound_load("sounds/382735__schots__gun-shot.wav", 1.0, 3);
        gfc_sound_play(gunshot, 0, 0.5, -1, -1);
        switch(self->attackType){
            case 0:
                //bullet
                rayCast(self, 50);
                break;
            case 1:
                //fire
                rayCast(self, 15);
                break;
            case 2:
                //melee
                rayCast(self, 5);
                break;
            case 3:
                //magic
                rayCast(self, 15);
                break;
            case 4:
                //ice
                rayCast(self, 20);
                break;
        }
        gfc_sound_free(gunshot);
    }
    
    if(keys[SDL_SCANCODE_F4]){
        //rotate 180 degrees instantly
        //used for testing since track pad is slow to turn
        self->rotation.z += M_PI;
    }
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
    

    //switch attacks, cycles through attacks
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

    //movement keys, with collision detection for the walls
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

    if(!self->isJumping && !self->isDescending){ //jump button, can only jump when not mid jump or mid descent
        if(keys[SDL_SCANCODE_SPACE]){
            slog("space pressed");
            self->startPosition = self->position.z;
            self->isJumping = 1;

            Sound *jump = gfc_sound_load("sounds/456369__felixyadomi__hop6.wav", 1.0, 2);
            gfc_sound_play(jump, 0, 1.0, -1, 1);
            gfc_sound_free(jump);
        }
    }

    //only able to go up and down when in edit mode
    if (keys[SDL_SCANCODE_X] && self->editMode == 1){//go up
        // if(collision_box_to_plane_z_up(self->bounds, topPlane) && !gfc_box_overlap(self->bounds, centerBox)){
        //     self->position.z+=1;
        // }
        // else{
        //     self->position.z-=1;
        // }
        self->position.z+=1;
    }
    if (keys[SDL_SCANCODE_Z] && self->editMode == 1){//go down
        // if(collision_box_to_plane_z_down(self->bounds, bottomPlane) && !gfc_box_overlap(self->bounds, centerBox)){
        //     self->position.z-=1;
        // }
        // else{
        //     self->position.z+=1;
        // }
        self->position.z-=1;
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
        slog("rot x: %f, rot y: %f, rot z: %f\n", self->rotation.x, self->rotation.y, self->rotation.z);
        slog("pos x: %f, pos y: %f, pos z: %f\n", self->position.x, self->position.y, self->position.z);
    }
    //keys for spawning monsters when in edit mode
    if(self->editMode == 1){
        if (keys[SDL_SCANCODE_1] && !keys[SDL_SCANCODE_LSHIFT]){
            monster1 = monster1_new(vector3d(self->position.x, self->position.y, self->position.z));
            if(monster1){
                entityList[i] = monster1;
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
                entityList[i] = monster2;
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
                entityList[i] = monster3;
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
                entityList[i] = monster4;
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
                entityList[i] = monster5;
                i++;
            }
        }
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
                entityList[i] = monster6;
                i++;
            }
        }
        if(keys[SDL_SCANCODE_6] && keys[SDL_SCANCODE_LSHIFT]){
            if(monster6){
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
                entityList[i] = monster7;
                i++;
            }
        }
        if(keys[SDL_SCANCODE_7] && keys[SDL_SCANCODE_LSHIFT]){
            if(monster7){
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
                entityList[i] = monster8;
                i++;
            }
        }
        if(keys[SDL_SCANCODE_8] && keys[SDL_SCANCODE_LSHIFT]){
            if(monster8){
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
                entityList[i] = monster9;
                i++;
            }
        }
        if(keys[SDL_SCANCODE_9] && keys[SDL_SCANCODE_LSHIFT]){
            if(monster9){
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
                entityList[i] = monster10;
                i++;
            }
        }
        if(keys[SDL_SCANCODE_0] && keys[SDL_SCANCODE_LSHIFT]){
            if(monster10){
                monster10->damage(self, monster10, self->attackDamage, 0);
                if(monster10->health == 0){
                    monster10->isDead = 1;
                    monster10 = NULL;
                }
            }
        }
    }

    //defense entities, spawn and tier up
    if(keys[SDL_SCANCODE_E] && !keys[SDL_SCANCODE_LALT]){ //spawn fence, cost = 100
        int cost = 100;
        int dif = self->points - cost;
        if(dif >= 0){ //buy it 
            fence = defense1_smallFence_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
            fence->state = ES_BASE;
            if(fence){
                entityList[i] = fence;
                i++;
            }
            self->defenseBounds[defenseCount] = fence->bounds;
            defenseCount++;
            self->defenseCount = defenseCount;
            wallList[m] = fence;
            m++;
            // fenceBounds = hitBoxEnt_new(self->position, fence);
            if(self->editMode == 0)
                self->points = dif;
        }
        else{
            slog("too poor, cant afford: smallFence");
        }
    }
    if(keys[SDL_SCANCODE_E] && keys[SDL_SCANCODE_LALT]){ //tier up fence, t1=150, t2=300, t3=500 
        if(fence){
            int t1 = 150, t2 = 300, t3 = 500, dif;
            switch(fence->state){
                case ES_BASE:
                    dif = self->points - t1;
                    if(dif >= 0){
                       fence->state = ES_T1;
                       fence->stateSwitched = 1;
                       self->points -= t1;
                    }
                    break;
                case ES_T1:
                    dif = self->points - t2;
                    if(dif >= 0){
                        fence->state = ES_T2;
                        fence->stateSwitched = 1;
                        self->points -= t2;
                    }
                    break;
                case ES_T2:
                    dif = self->points - t3;
                    if(dif >= 0){
                        fence->state = ES_T3;
                        fence->stateSwitched = 1;
                        self->points -= t3;
                    }
                    break;

            }
        }
    }
    if(keys[SDL_SCANCODE_R] && !keys[SDL_SCANCODE_LALT]){ //spawn wall, cost = 400
        int cost = 400;
        int dif = self->points - cost;
        if(dif >= 0){
            wall = defense2_smallWall_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
            if(wall){
                entityList[i] = wall;
                i++;
            }
            self->defenseBounds[defenseCount] = wall->bounds;
            self->defenseCount = defenseCount;
            defenseCount++;
            wallList[m] = fence;
            m++;
            self->points = dif;
        }
        else{
            slog("too poor, cant afford: smallWall");
        }
    }
    if(keys[SDL_SCANCODE_R] && keys[SDL_SCANCODE_LALT]){ //tier up the wall, t1=600, t2=850, t3=1200
        if(wall){
            int t1 = 600, t2 = 850, t3 = 1200, dif;
            slog("fnornf: %i", wall->state);
            switch(wall->state){
                case ES_BASE:
                    dif = self->points - t1;
                    if(dif >= 0){
                       wall->state = ES_T1;
                       wall->stateSwitched = 1;
                       self->points -= t1;
                    }
                    break;
                case ES_T1:
                    dif = self->points - t2;
                    if(dif >= 0){
                        wall->state = ES_T2;
                        wall->stateSwitched = 1;
                        self->points -= t2;
                    }
                    break;
                case ES_T2:
                    dif = self->points - t3;
                    if(dif >= 0){
                        wall->state = ES_T3;
                        wall->stateSwitched = 1;
                        self->points -= t3;
                    }
                    break;

            }
        }
    }
    if(keys[SDL_SCANCODE_T] && !keys[SDL_SCANCODE_LALT]){ //spawn turret1, cost = 250, middle tier
        int cost = 250;
        int dif = self->points - cost;
        if(dif >= 0){
            turret1 = defense3_turret1_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
            if(turret1){
                entityList[i] = turret1;
                i++;
            }
            self->defenseBounds[defenseCount] = turret1->bounds;
            self->defenseCount = defenseCount;
            defenseCount++;
            self->points = dif;
        }
        else{
            slog("too poor, cant afford: turret1");
        }
    }
    if(keys[SDL_SCANCODE_T] && keys[SDL_SCANCODE_LALT]){ //tier up turret1, t1=450, t2=650, t3=900
        if(turret1){
            int t1 = 450, t2 = 650, t3 = 900, dif;
            switch(turret1->state){
                case ES_BASE:
                    dif = self->points - t1;
                    if(dif >= 0){
                       turret1->state = ES_T1;
                       turret1->stateSwitched = 1;
                       self->points -= t1;
                    }
                    break;
                case ES_T1:
                    dif = self->points - t2;
                    if(dif >= 0){
                        turret1->state = ES_T2;
                        turret1->stateSwitched = 1;
                        self->points -= t2;
                    }
                    break;
                case ES_T2:
                    dif = self->points - t3;
                    if(dif >= 0){
                        turret1->state = ES_T3;
                        turret1->stateSwitched = 1;
                        self->points -= t3;
                    }
                    break;

            }
        }
    }
    if(keys[SDL_SCANCODE_Y] && !keys[SDL_SCANCODE_LALT]){ //spawn turret2, cost = 500, top tier
        int cost = 500;
        int dif = self->points - cost;
        if(dif >= 0){
            turret2 =defense4_turret2_new(vector3d(self->position.x, self->position.y, self->position.z - 5), self->rotation);
            if(turret2){
                entityList[i] = turret2;
                i++;
            }
            self->defenseBounds[defenseCount] = turret2->bounds;
            self->defenseCount = defenseCount;
            defenseCount++;
            self->points = dif;
        }
        else{
            slog("too poor, cant afford: turret2");
        }
    }
    if(keys[SDL_SCANCODE_Y] && keys[SDL_SCANCODE_LALT]){ //tier up turret2, t1=750, t2=1000, t3 = 1650
        if(turret2){
            int t1 = 750, t2 = 1000, t3 = 1650, dif;
            switch(turret2->state){
                case ES_BASE:
                    dif = self->points - t1;
                    if(dif >= 0){
                       turret2->state = ES_T1;
                       turret2->stateSwitched = 1;
                       self->points -= t1;
                    }
                    break;
                case ES_T1:
                    dif = self->points - t2;
                    if(dif >= 0){
                        turret2->state = ES_T2;
                        turret2->stateSwitched = 1;
                        self->points -= t2;
                    }
                    break;
                case ES_T2:
                    dif = self->points - t3;
                    if(dif >= 0){
                        turret2->state = ES_T3;
                        turret2->stateSwitched = 1;
                        self->points -= t3;
                    }
                    break;

            }
        }
    }
    if(keys[SDL_SCANCODE_U] && !keys[SDL_SCANCODE_LALT]){ //spawn turrent3, cost = 150, bottom tier
        int cost = 150;
        int dif = self->points - cost;
        if(dif >= 0){
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
            self->points = dif;
        }
        else{
            slog("too poor, cant afford: turret3");
        }
    }
    if(keys[SDL_SCANCODE_U] && keys[SDL_SCANCODE_LALT]){ //tier up turret3, t1=300, t2=500, t3=1000
        if(turret3){
            int t1 = 300, t2 = 500, t3 = 1000, dif;
            switch(turret3->state){
                case ES_BASE:
                    dif = self->points - t1;
                    if(dif >= 0){
                       turret3->state = ES_T1;
                       turret3->stateSwitched = 1;
                       self->points -= t1;
                    }
                    break;
                case ES_T1:
                    dif = self->points - t2;
                    if(dif >= 0){
                        turret3->state = ES_T2;
                        turret3->stateSwitched = 1;
                        self->points -= t2;
                    }
                    break;
                case ES_T2:
                    dif = self->points - t3;
                    if(dif >= 0){
                        turret3->state = ES_T3;
                        turret3->stateSwitched = 1;
                        self->points -= t3;
                    }
                    break;

            }
        }
    }

    while(SDL_PollEvent(&testEvent) > 0){
        //slog("in SDL_PollEvent");
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
    if(keys[SDL_SCANCODE_Q] && !keys[SDL_SCANCODE_LSHIFT]){//save game in saves/entityList.json and playerSave.json
        //test json stuff
        SJson *body = sj_object_new();

        SJson *playerEnt = sj_object_new();
        playerSave(self, playerEnt);
        sj_save(playerEnt, "saves/playerSave.json");
        
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
    if(keys[SDL_SCANCODE_Q] && keys[SDL_SCANCODE_LSHIFT]){//test load, can be used but mostley done from main menu
        //loadEntity(self);
        // slog("start load");
        // SJson *body = sj_object_new();
        // body = sj_load("saves/entityList.json");

        // SJList *keysList = sj_object_get_keys_list(body);

        // //load player info
        // SJson *playerEnt = sj_object_new();
        // playerEnt = sj_object_get_value(body, sj_list_get_nth(keysList, 0));
        // playerLoad(self, playerEnt);
        // slog("isPlayer: %i", self->isPlayer);

        // slog("keyList size: %i, count: %i",keysList->size, keysList->count);
        // //entityList = NULL; //reset entityList

        // //reset entityList to 0, only needed when loading while already playing
        // for(int j=0; j<512; j++){
        //     entityList[j] == NULL;
        // }
        // i=0;
        
        // for(int j=1; j<keysList->count; j++){
        //     SJson *entity = sj_object_new();
        //     entity = sj_object_get_value(body, sj_list_get_nth(keysList, j));
        //     Entity *temp = entity_new();
        //     // int *type = NULL, *num = NULL, *isPlayer = NULL;
        //     // float *position_x = NULL, *position_y = NULL, *position_z = NULL, *rotation_x = NULL, *rotation_y = NULL, *rotation_z = NULL;
        //     sj_get_integer_value(sj_object_get_value(entity, "entityType"), &temp->type);
        //     sj_get_integer_value(sj_object_get_value(entity, "entityNum"), &temp->entityNum);
        //     // sj_get_integer_value(sj_object_get_value(entity, "isPlayer"), &temp->isPlayer);

        //     //need to pull position/rotation data to spawn some entities
        //     sj_get_float_value(sj_object_get_value(entity, "position_x"), &temp->position.x);
        //     sj_get_float_value(sj_object_get_value(entity, "position_y"), &temp->position.y);
        //     sj_get_float_value(sj_object_get_value(entity, "position_z"), &temp->position.z);

        //     sj_get_float_value(sj_object_get_value(entity, "rotation_x"), &temp->rotation.x);
        //     sj_get_float_value(sj_object_get_value(entity, "rotation_y"), &temp->rotation.y);
        //     sj_get_float_value(sj_object_get_value(entity, "rotation_z"), &temp->rotation.z);

        //     // /slog("Type: %i, Num: %i", type, num);

        //     // Vector3D position = vector3d(position_x, position_y, position_z);
        //     // Vector3D rotation = vector3d(rotation_x, rotation_y, rotation_z);
        //     // slog("position: x: %f, y:%f, z:%f", *position_x, *position_y, *position_z);
        //     // slog("rotation: x: %f, y:%f, z:%f", *rotation_x, *rotation_y, *rotation_z);

        //     Entity *newEnt;
        //     if(temp->isPlayer == 1){
        //         //is the player
        //     }
        //     else if(temp->type == 0){
        //         //is a monster
        //         switch(temp->entityNum){
        //             case 1:
        //                 newEnt = NULL;
        //                 newEnt = monster1_new(temp->position);
        //                 slog("spawning monster1, EntityType: %i", temp->type);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 2:
        //                 newEnt = NULL;
        //                 newEnt = monster2_kong_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 3:
        //                 newEnt = NULL;
        //                 newEnt = monster3_porygon_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 4:
        //                 newEnt = NULL;
        //                 newEnt = monster4_skelly_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 5:
        //                 newEnt = NULL;
        //                 newEnt = monster5_mario_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 6:
        //                 newEnt = NULL;
        //                 newEnt = monster6_yoshi_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 7:
        //                 newEnt = NULL;
        //                 newEnt = monster7_creeper_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 8:
        //                 newEnt = NULL;
        //                 newEnt = monster8_finn_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 9:
        //                 newEnt = NULL;
        //                 newEnt = monster9_goomba_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 10:
        //                 newEnt = NULL;
        //                 newEnt = monster10_arlo_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //         }
        //     }
        //     else if(temp->type == 1){
        //         //is a wall
        //         switch(temp->entityNum){
        //             case 1:
        //                 newEnt = entity_new();
        //                 temp->rotation.z += (M_PI/2.0);
        //                 newEnt = defense1_smallFence_new(temp->position, temp->rotation);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 2:
        //                 newEnt = NULL;
        //                 temp->rotation.z += (M_PI/2.0);
        //                 newEnt = defense2_smallWall_new(temp->position, temp->rotation);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //         }
        //     }
        //     else if(temp->type == 2){
        //         //is a turret
        //         switch(temp->entityNum){
        //             case 1:
        //                 newEnt = NULL;
        //                 temp->rotation.z += (M_PI/2.0);
        //                 newEnt = defense3_turret1_new(temp->position, temp->rotation);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 2:
        //                 newEnt = NULL;
        //                 temp->rotation.z += (M_PI/2.0);
        //                 newEnt = defense4_turret2_new(temp->position, temp->rotation);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 3:
        //                 newEnt = NULL;
        //                 temp->rotation.z += (M_PI/2.0);
        //                 newEnt = defense5_turret3_new(temp->position, temp->rotation);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //         }
        //     }
        //     else if(temp->type == 3){
        //         //is goo
        //         switch(temp->entityNum){
        //             case 1:
        //                 newEnt = NULL;
        //                 newEnt = slowGoo_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 2:
        //                 newEnt = NULL;
        //                 newEnt = fastGoo_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 3:
        //                 newEnt = NULL;
        //                 newEnt = dmgGoo_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 4:
        //                 newEnt = NULL;
        //                 newEnt = healGoo_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //             case 5:
        //                 newEnt = NULL;
        //                 newEnt = incDmgGoo_new(temp->position);
        //                 newEnt = testLoad(newEnt, entity);
        //                 entityList[j] = newEnt;
        //                 i++;
        //                 break;
        //         }
        //     }
        // }

        // slog("load done");
    }
    if(keys[SDL_SCANCODE_O]){ //loop and print through entity list, type of entities
        for(int j=0; j<i; j++){
            slog("entityList[j]->type: %i", entityList[j]->type);
        }
    }
    if(keys[SDL_SCANCODE_P]){ //NOT USED, test player save
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
    if(keys[SDL_SCANCODE_L]){ //NOT USED, test player load
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
    if(keys[SDL_SCANCODE_RCTRL]){//loop through entity list and print type and num
        for(int j=1; j<i; j++){
            slog("entityType: %i", entityList[j]->type);
            slog("entityNum: %i", entityList[j]->entityNum);
        }
    }

    if(keys[SDL_SCANCODE_F1]){//start audio for test
        Sound *music = gfc_sound_load("music/209561__dneproman__8-bit-style.wav", 1.0, 1);
        gfc_sound_play(music, 0, 1.0, -1, 1);
        gfc_sound_free(music);
    }
    
}
int lastSpawn = 0;
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
    if(self->editMode == 1){
        //dont do gravity, player can noclip around and place monsters and entities how they want to.
    }
    else if(collision_box_to_plane_z_down(self->bounds, bottomPlane)){
        //do gravity and check collisions
        if(!gfc_box_overlap(self->bounds, centerBox)){
            if(!(self->isJumping || self->isDescending)){
                //if not touching the ground, the center box, or jumping/descending do gravity
                self->position.z -= 1;
            }
        }
    }


    //check for goo effects
    if(dmgGoo){//do dmg to all ents
        //slog("dmgGoo exists");
        for(int j=1; j<i; j++){
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
    if(healGoo){//heal only player
        if(gfc_box_overlap(self->bounds, healGoo->bounds)){
            entity_damage(healGoo, self, 1, 1);
        }
    }
    if(incDmgGoo){//double damage for player only
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

    //sync defense list to all entities
    for(int j=0; j<i; j++){
        for(int x=0; x<200; x++){
            entityList[j]->defenseBounds[x] = self->defenseBounds[x];
        }
    }
    Box gone;
    //have entity do dmg to the wall that they are currently attacking
    for(int j=0; j<i; j++){
        //if there is a wall in front of the entity
        if(entityList[j]->defenseObj.x != 0 && entityList[j]->defenseObj.y != 0 && entityList[j]->defenseObj.z != 0){
            for(int l=0; l<m; l++){
                //found equivalent wall entity
                if(wallList[l]->bounds.x == entityList[j]->defenseObj.x && wallList[l]->bounds.y == entityList[j]->defenseObj.y && wallList[l]->bounds.z == entityList[j]->defenseObj.z && wallList[l] != NULL){
                    entity_damage(entityList[j], wallList[l], 15, 0);
                    if(wallList[l] != NULL && wallList[l]->health == 0){
                        gone = wallList[l]->bounds;
                        wallList[l] = NULL;
                        wallDestroyed = 1;
                    }
                    else{
                        //kept getting seg faults so added else that should prevent it, since it was doing the op on wallList[l] after it was set to NULL
                        slog("wall health; %i", wallList[l]->health);
                    }
                }
            }
        }
    }
    //when the wall is destroyed, set it bounds to 0 in the defenseBounds array
    //monsters use this array to check if they are coming into contact with a wall
    if(wallDestroyed == 1){
        for(int j=0; j<defenseCount; j++){
            if(self->defenseBounds[j].x == gone.x && self->defenseBounds[j].y == gone.y && self->defenseBounds[j].z == gone.z){
                self->defenseBounds[j].d = 0;
                self->defenseBounds[j].h = 0;
                self->defenseBounds[j].w = 0;
                self->defenseBounds[j].x = 0;
                self->defenseBounds[j].y = 0;
                self->defenseBounds[j].z = 0;
                wallDestroyed = 0;
            }
        }
    }

    if(self->health == 0){
        //slog("self->isDead == %i", self->isDead);
        //you died
        self->isDead = 1;
    }
}

void player_damage(int damage, Entity *self, int heal, Entity *inflictor){
    if(heal == 0){//damage not heal
        int temp = self->health - damage;
        //slog("temp1: %i", temp);
        if(temp < 0){
            temp = 0;
        }
        //slog("temp2: %i", temp);
        if(self->editMode != 1) self->health = temp;
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

//loads all ents including the player ent
//TODO: need to also save the current game mode(edit mode) so it loads into the correct mode when saving, maybe give option on main menu to choose to load into default or edit mode from a save
void loadEntities(Entity *self){ //loads entities from json and spawns them 
    slog("start load");
        SJson *body = sj_object_new();
        body = sj_load("saves/entityList.json");

        SJList *keysList = sj_object_get_keys_list(body);

        //load player info
        
        SJson *playerEnt = sj_object_new();
        playerEnt = sj_load("saves/playerSave.json");
        // playerEnt = sj_object_get_value(body, sj_list_get_nth(keysList, 0));
        playerLoad(self, playerEnt);
        // slog("isPlayer: %i", self->isPlayer);

        slog("keyList size: %i, count: %i",keysList->size, keysList->count);
        //entityList = NULL; //reset entityList

        //reset entityList to 0, only needed when loading while already playing
        for(int j=0; j<512; j++){
            entityList[j] == NULL;
        }
        i=0;
        
        for(int j=0; j<keysList->count; j++){
            SJson *entity = sj_object_new();
            entity = sj_object_get_value(body, sj_list_get_nth(keysList, j));
            Entity *temp = entity_new();
            // int *type = NULL, *num = NULL, *isPlayer = NULL;
            // float *position_x = NULL, *position_y = NULL, *position_z = NULL, *rotation_x = NULL, *rotation_y = NULL, *rotation_z = NULL;
            sj_get_integer_value(sj_object_get_value(entity, "entityType"), &temp->type);
            sj_get_integer_value(sj_object_get_value(entity, "entityNum"), &temp->entityNum);
            // sj_get_integer_value(sj_object_get_value(entity, "isPlayer"), &temp->isPlayer);

            //need to pull position/rotation data to spawn some entities
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

int x = 0, waveCount = 0;
void player_spawnWave(Vector3D spawnPos, Entity *player){
    player->currentWave = waveCount;
    Entity *newEnt = NULL;
    srand(time(NULL));
    time(&currentTime);
    time_t dif = currentTime - lastTime;//if dif = 1, then thats 10 sec
    spawnPos.z -= 3;
    if(dif>=2 && player->editMode != 1){
        // newEnt = monster7_creeper_new(spawnPos);
        // lastTime = currentTime;
        // entityList[i] = newEnt;
        // i++;
        if(waveCount % 5 != 0){
            for(int m = 0; m<waveCount; m++){
                Vector3D newSpawnPos = {0};
                if (m != 0) newSpawnPos.x += 4;
                int num = (rand() % 10); //get a random number 0-9 to spawn a random enemy
                switch (num){
                    case 0:
                        newSpawnPos = spawnPos;
                        newSpawnPos.z += 1;
                        newEnt = monster1_new(newSpawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 1:
                        newEnt = monster2_kong_new(spawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 2:
                        newSpawnPos = spawnPos;
                        newSpawnPos.z += 5;
                        newEnt = monster3_porygon_new(newSpawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 3:
                        newSpawnPos = spawnPos;
                        newSpawnPos.z += 1;
                        newEnt = monster4_skelly_new(newSpawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 4:
                        newEnt = monster5_mario_new(spawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 5:
                        newSpawnPos = spawnPos;
                        newSpawnPos.z += 2;
                        newEnt = monster7_creeper_new(newSpawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 6:
                        newSpawnPos = spawnPos;
                        newSpawnPos.z += 3;
                        newEnt = monster8_finn_new(newSpawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 7:
                        newSpawnPos = spawnPos;
                        newSpawnPos.z -= 0.5;
                        newEnt = monster9_goomba_new(newSpawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 8:
                        newSpawnPos = spawnPos;
                        newSpawnPos.z += 2;
                        newEnt = monster10_arlo_new(newSpawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                    case 9:
                        newEnt = monster6_yoshi_new(spawnPos);
                        lastTime = currentTime;
                        entityList[i] = newEnt;
                        i++;
                        break;
                }
                spawnPos.x -= 2;
            }
        }        
        waveCount++;
        if(waveCount % 5 == 0 && newEnt != NULL){ //every 5 waves, make the enemy that spawns a boss by raising the scale and damage, setting it to a boss will raise the damage
            Vector3D currentScale = newEnt->scale;
            vector3d_scale(newEnt->scale, currentScale, 4);
            newEnt->isBoss = 1;
            newEnt->health *= 3; //triple normal health
            newEnt->attackDamage *=4; //double normal attack
            newEnt->position.z += 10; //gets set in the ground so raise it up a little bit
            newEnt->bounds.z -= 10; //change to location of the bottom of the bounding box
            newEnt->bounds.d *= 2; //update the bounds for the new size of the model
            newEnt->bounds.h *= 2;
            newEnt->bounds.w *= 2;
            
            slog("BOSS SPAWNING!!");
            player->bossSpawning = 1;
        }
        
        x++;

        slog("spawned one: %i", x);
    }



    // slog("currentTime(%ld) - lastTime(%ld) = %ld",currentTime, lastTime, dif);
}

void rayCast(Entity *self, float distance){
    //self is the player entity
    //seems to work when trying to raycast into the -y side, very inconsistent when ray casting into the +y side
    Vector3D playerPos = self->position;
    playerPos.z += 7.5;
    //float distance = 5.0; //10 actually makes a distance of 25, 5 = 12.5 distance and so on
    float pitch = self->rotation.x;
    float yaw = self->rotation.z;
    float sp = sinf(pitch);
    float cp = cosf(pitch) - M_PI_2;
    float sy = sinf(yaw - M_PI_2);
    float cy = cosf(yaw - M_PI_2);

    Vector3D dir = vector3d(cp*cy, cp*sy, sp);
    float x = playerPos.x + dir.x * distance;
    float y = playerPos.y + dir.y * distance;
    float z = playerPos.z + dir.z * distance;
    Vector3D testPoint = vector3d(x, y, z);
    slog("testPoint: %f, %f, %f", x, y, z);
    Edge3D ray; //more like a line segment then a ray
    ray.a = playerPos;
    ray.b = testPoint;
    Vector3D *poc = NULL, *normal = NULL;
    slog("edge length: %f", edge3DLength(ray));
    // self->position = testPoint;
    for(int j=0; j<i; j++){
        //slog("in entity loop");
        //slog("Entity[j] bounding box: %f, %f, %f\n%f, %f, %f", entityList[j]->bounds.x, entityList[j]->bounds.y, entityList[j]->bounds.z, entityList[j]->bounds.d, entityList[j]->bounds.w, entityList[j]->bounds.h);
        // Box bound;
        // bound.x = entityList[j]->bounds.x;
        // bound.y = entityList[j]->bounds.y;
        // bound.z = entityList[j]->bounds.z;
        // bound.h = 10;
        // bound.d = 5;
        // bound.w = 5;
        //slog("bound box: %f, %f, %f\n%f, %f, %f", bound.x, bound.y, bound.z, bound.d, bound.w, bound.h);

        if(gfc_edge_box_test(ray, entityList[j]->bounds, poc, normal) && entityList[j]->type == ET_monster){
            slog("intersected with entity type: %i", entityList[j]->type);
            slog("intersected entity pos: %f, %f, %f", entityList[j]->position.x, entityList[j]->position.y, entityList[j]->position.z);
            entity_damage(self, entityList[j], self->attackDamage, 0);
            //return entityList[j];
        }
        else if(gfc_edge_box_test(ray, entityList[j]->bounds, poc, normal) && entityList[j]->type != ET_monster){
            slog("interected with non-monster ent");
        }
    }
}
/*eol@eof*/
