
#include "simple_logger.h"
#include "monster3_porygon.h"


void monster3_porygon_update(Entity *self);

void monster3_porygon_think(Entity *self);

Entity *monster3_porygon_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no monster3_porygon for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/monster3_porygon.model");
    ent->think = monster3_porygon_think;
    ent->update = monster3_porygon_update;
    ent->scale = vector3d(0.1,0.1,0.1);
    vector3d_copy(ent->position,position);
    return ent;
}

void monster3_porygon_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    self->rotation.z += 0.01;
}

void monster3_porygon_think(Entity *self)
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