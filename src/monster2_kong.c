
#include "simple_logger.h"
#include "monster2_kong.h"
#include "collision.h"
#include "player.h"


void monster2_kong_update(Entity *self, Entity *player);

void monster2_kong_think(Entity *self);

Entity *monster2_kong_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no monster2_kong for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1);
    ent->color = gfc_color(1,1,1,1);
    ent->scale = vector3d(5,5,5);
    ent->model = gf3d_model_load("models/monster2_kong.model");
    ent->think = monster2_kong_think;
    ent->update = monster2_kong_update;
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;

    vector3d_copy(ent->position,position);

    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.w = 5;
    ent->bounds.h = 7;
    ent->bounds.d = 5;

    ent->health = 100;

    ent->type = ET_monster;
    ent->entityNum = 2;
    ent->attackDamage = 1;
    return ent;
}

void monster2_kong_update(Entity *self, Entity *player)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);

    Vector3D boxPos = vector3d(self->position.x - (self->bounds.w / 2), self->position.y - (self->bounds.h / 2.0), self->position.z);
    
    
    self->bounds.x = self->position.x;
    self->bounds.y = self->position.y;
    self->bounds.z = self->position.z+2;

    if(self->isBoss == 1){
        self->bounds.z = self->position.z - 6;
    }

    // self->bounds.x = boxPos.x;
    // self->bounds.y = boxPos.y;
    // self->bounds.z = boxPos.z;

    Vector2D facingVec;
    vector2d_sub(facingVec, player->position, self->position);
    float rotate = atan2(facingVec.y, facingVec.x);
    self->rotation.z = rotate + M_PI;

    Box centerBox = gfc_box(0,0,-25, 10,10,10);
    Plane3D bottomPlane = gfc_plane3d(0,0,-25,25);
    if(collision_box_to_plane_z_down(self->bounds, bottomPlane) && !gfc_box_overlap(self->bounds, centerBox)){ //check for collison on gound and center box
        if(!(self->position.z <= -25) && (self->isBoss == 0)){
            self->position.z -=1;
        }
    }
    

    Vector2D *selfPos = NULL;
    Vector2D vect = vector2d(self->position.x, self->position.y);
    selfPos = &vect;
    Vector2D playerPos = vector2d(player->position.x, player->position.y);
    if(!gfc_box_overlap(player->bounds, self->bounds) && player->editMode != 1){ //if the player is in edit mode, don't let the AI move from their spawn location
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
                slog("stuck beind wall, now damaging it");
                self->behindWall = 1;
            }
        }
    }
    else{
        // enemy is touching player
        player_damage(self->attackDamage, player, 0, self);
    }
    //self->rotation.z += 0.01;
}

void monster2_kong_think(Entity *self)
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
