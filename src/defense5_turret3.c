
#include "simple_logger.h"
#include "defense5_turret3.h"
#include "gfc_types.h"


void defense5_turret3_update(Entity *self, Vector3D playerPos);

void defense5_turret3_think(Entity *self);

Entity *defense5_turret3_new(Vector3D position, Vector3D rotation)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no defense5_turret3 for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/defense5_turret3.model");
    ent->think = defense5_turret3_think;
    ent->update = defense5_turret3_update;
    ent->scale = vector3d(1,1,1);
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;

    vector3d_copy(ent->position,position);

    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.w = 10;
    ent->bounds.h = 10;
    ent->bounds.d = 5;

    ent->health = 100;
    //slog("%f", rotation.z);
    ent->rotation.z = rotation.z + 1.57; //rotate 90 degrees so it spawns perpendicular to where the player is looking
    //ent->rotation.y = rotation.y;
    return ent;
    ent->selectedColor = gfc_color(1, 0.5, 0, 0.9);
}

void defense5_turret3_update(Entity *self, Vector3D playerPos)
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
    
}

void defense5_turret3_think(Entity *self)
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
