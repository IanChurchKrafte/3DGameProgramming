#include "simple_logger.h"
#include "slowGoo.h"

void slowGoo_update(Entity *self);

Entity *slowGoo_new(Vector3D position){
    Entity *ent = NULL;

    ent = entity_new();
    if(!ent){
        slog("cant spawn goo");
        return NULL;
    }

    ent->model = gf3d_model_load("models/splat1.model");
    ent->update = slowGoo_update;
    ent->scale = vector3d(1,1,1);
    ent->onDeath = entity_onDeath;
    vector3d_copy(ent->position, position);
    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.h = 3;
    ent->bounds.w = 10;
    ent->bounds.d = 10;

    return ent;
}

void slowGoo_update(Entity *self){
    return;
}