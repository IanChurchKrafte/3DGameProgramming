#include "simple_logger.h"
#include "dmgGoo.h"

void dmgGoo_update(Entity *self, Entity *player);

Entity *dmgGoo_new(Vector3D position){
    Entity *ent = NULL;

    ent = entity_new();
    if(!ent){
        slog("cant spawn dmgGoo");
        return NULL;
    }

    ent->model = gf3d_model_load("models/dmgGoo.model");
    ent->update = dmgGoo_update;
    ent->scale = vector3d(1,1,1);
    ent->onDeath = entity_onDeath;
    position.z -= 5;
    vector3d_copy(ent->position, position);
    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.h = 3;
    ent->bounds.w = 10;
    ent->bounds.d = 10;

    ent->type = ET_goo;
    ent->entityNum = 3;

    return ent;
}

void dmgGoo_update(Entity *self, Entity *player){
    return;
}