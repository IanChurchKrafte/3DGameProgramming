#include "simple_logger.h"
#include "hitBoxEnt.h"
#include "math.h"


void hitBoxEnt_update(Entity *self, Entity *ent);

void monster7_creeper_think(Entity *self);

Entity *hitBoxEnt_new(Vector3D position, Entity *follow)
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
    ent->model = gf3d_model_load("models/hitbox.model");
    ent->think = monster7_creeper_think;
    ent->update = hitBoxEnt_update;
    ent->scale = vector3d(1,1,1);
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;

    vector3d_copy(ent->position,position);

    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->scale.x = follow->bounds.w/2;
    ent->scale.y = follow->bounds.d/2;
    ent->scale.z = follow->bounds.h/2;

    ent->health = 100;

    return ent;
}

void hitBoxEnt_update(Entity *self, Entity *follow){
    self->position.x = follow->bounds.x;
    self->position.y = follow->bounds.y;
    self->position.z = follow->bounds.z;

    
}