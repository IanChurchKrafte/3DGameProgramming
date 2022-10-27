#include "simple_logger.h"
#include "monster1.h"

void monster1_update(Entity *self);
void monster1_think(Entity *self);

Entity *monster1_new(Vector3D position){
    Entity *ent = NULL;

    ent = entity_new();
    if(!ent){
        slog("cant spawn monster1");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model=gf3d_model_load("models/charmander.model");
    //Vector3D scale = (1.5,1.5,1.5);
    ent->scale = vector3d(2.5,2.5,2.5);
    ent->think = monster1_think;
    ent->update = monster1_update;
    ent->rotation.x+=1.5;
    vector3d_copy(ent->position,position);
    return ent;
}

void monster1_update(Entity *self){
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
}

void monster1_think(Entity *self){
    if(!self){
        slog("self pointer not provided");
        return;
    }
    //self->rotation.x += -0.005;
}

