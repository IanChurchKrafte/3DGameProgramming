#include "simple_logger.h"
#include "turret1.h"

void turret1_update(Entity *self);
void turret1_think(Entity *self);

Entity *turret1_new(Vector3D position){
    Entity *ent = NULL;

    ent = entity_new();
    if(!ent){
        slog("cant spawn turret1");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model=gf3d_model_load("models/turret1.model");
    ent->think = turret1_think;
    ent->update = turret1_update;
    vector3d_copy(ent->position,position);
    return ent;
}

void turret1_update(Entity *self){
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
}

void turret1_think(Entity *self){
    if(!self){
        slog("self pointer not provided");
        return;
    }
    //self->rotation.x += -0.005;
}
