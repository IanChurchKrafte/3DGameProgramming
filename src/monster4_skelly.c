
#include "simple_logger.h"
#include "monster4_skelly.h"
#include "collision.h"


void monster4_skelly_update(Entity *self, Entity *player);

void monster4_skelly_think(Entity *self);

Entity *monster4_skelly_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no monster2_kong for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/monster4_skelleton.model");
    ent->think = monster4_skelly_think;
    ent->update = monster4_skelly_update;
    ent->scale = vector3d(3,3,3);
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;

    vector3d_copy(ent->position,position);

    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.w = 10;
    ent->bounds.h = 10;
    ent->bounds.d = 10;

    ent->health = 100;

    ent->type = ET_monster;
    ent->entityNum = 4;
    return ent;
}

void monster4_skelly_update(Entity *self, Entity *player)
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

    Vector2D facingVec;
    vector2d_sub(facingVec, player->position, self->position);
    float rotate = atan2(facingVec.y, facingVec.x);
    self->rotation.z = rotate + M_PI;

    Box centerBox = gfc_box(0,0,-25, 10,10,10);
    Plane3D bottomPlane = gfc_plane3d(0,0,-25,25);
    if(collision_box_to_plane_z_down(self->bounds, bottomPlane) && !gfc_box_overlap(self->bounds, centerBox)){ //check for collison on gound and center box
        self->position.z -=1;
    }
    
    //self->rotation.z += 0.01;
}

void monster4_skelly_think(Entity *self)
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
