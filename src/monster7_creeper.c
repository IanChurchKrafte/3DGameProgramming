
#include "simple_logger.h"
#include "monster7_creeper.h"
#include "collision.h"
#include "gfc_vector.h"
#include "math.h"


void monster7_creeper_update(Entity *self, Entity *player);

void monster7_creeper_think(Entity *self);

Entity *monster7_creeper_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no monster7_creeper for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/monster7_creeper.model");
    ent->think = monster7_creeper_think;
    ent->update = monster7_creeper_update;
    ent->scale = vector3d(2,2,2);
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;
    position.z -= 2;
    ent->rotation.z = 0.1;
    //ent->rotation.x = 1.5;
    vector3d_copy(ent->position,position);

    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.w = 5;
    ent->bounds.h = 5;
    ent->bounds.d = 5;

    ent->type = ET_monster;
    ent->entityNum = 7;

    ent->health = 100;
    ent->behindWall = 0;

    ent->resistance = 2; //resistant to melee damage;
    ent->selectedColor = gfc_color(0.96, 0.16, 0.57, 0.8); //pink
    ent->selected = 1;
    return ent;
}

void monster7_creeper_update(Entity *self, Entity *player)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    
    self->bounds.x = self->position.x;
    self->bounds.y = self->position.y;
    self->bounds.z = self->position.z;
    
    //self->rotation.z += 0.01;
    Box centerBox = gfc_box(0,0,-25, 10,10,10);
    Plane3D bottomPlane = gfc_plane3d(0,0,-25,25);
    if(collision_box_to_plane_z_down(self->bounds, bottomPlane) && !gfc_box_overlap(self->bounds, centerBox)){ //check for collison on gound and center box
        //self->position.z -=1;
    }
    //trying to get it to rotate towards the player since im already moving in that direction
    Vector2D facingVec;
    vector2d_sub(facingVec, player->position, self->position);
    float rotate = atan2(facingVec.y, facingVec.x);
    self->rotation.z = rotate + M_PI;

    Vector2D *selfPos = NULL;
    Vector2D vect = vector2d(self->position.x, self->position.y);
    selfPos = &vect;
    Vector2D playerPos = vector2d(player->position.x, player->position.y);
    if(!gfc_box_overlap(player->bounds, self->bounds)){
        //slog("not colliding with player");
        //add entity list for entities spawn in by the player
        if(player->defenseCount == 0){
            //slog("no defenses found");
            vector2d_move_towards(selfPos, vector2d(self->position.x, self->position.y), playerPos, 0.5);
            self->position.x = selfPos->x;
            self->position.y = selfPos->y;
        }
        for(int i=0; i<player->defenseCount; i++){
            //slog("knows that a fence is here");
            if(!gfc_box_overlap(player->defenseBounds[i], self->bounds)){
                //slog("not colliding with defense entity");
                vector2d_move_towards(selfPos, vector2d(self->position.x, self->position.y), playerPos, 0.5);
                self->position.x = selfPos->x;
                self->position.y = selfPos->y;
                //return 1;
            }
            else{
                //slog("stuck beind wall, now damaging it");
                self->behindWall = 1;
            }
        }
    }
    else{
        // enemy is touching player
        // player->health -= 1;
    }
}

void monster7_creeper_think(Entity *self)
{
    if (!self)return;
    switch(self->state)
    {
        case ES_idle:
            //look for player
            break;
        case ES_hunt:
            // set move towards player
            break;
        case ES_dead:
            // remove myself from the system
            break;
        case ES_attack:
            // run through attack animation / deal damage
            break;
        case ES_T1:
            break;
        case ES_T2:
            break;
        case ES_T3:
            break;
        case ES_BASE:
            break;
    }
}

/*eol@eof*/
