
#include "simple_logger.h"
#include "monster5_mario.h"


void monster5_mario_update(Entity *self);

void monster5_mario_think(Entity *self);

Entity *monster5_mario_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no monster5_mario for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/monster5_mario.model");
    ent->think = monster5_mario_think;
    ent->update = monster5_mario_update;
    ent->scale = vector3d(2,2,2);
    ent->rotation.y += 1.5;
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;

    vector3d_copy(ent->position,position);

    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.w = ent->scale.x+10;
    ent->bounds.h = ent->scale.z+10;
    ent->bounds.d = ent->scale.y+10;

    ent->health = 100;
    return ent;
}

void monster5_mario_update(Entity *self)
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
    self->bounds.w = 10;
    self->bounds.h = 10;
    self->bounds.d = 10;
    
    self->rotation.z += 0.01;
}

void monster5_mario_think(Entity *self)
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
    }
}

/*eol@eof*/