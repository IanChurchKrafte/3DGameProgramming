
#include "simple_logger.h"
#include "monster6_yoshi.h"


void monster6_yoshi_update(Entity *self);

void monster6_yoshi_think(Entity *self);

Entity *monster6_yoshi_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no monster6_yoshi for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/monster6_yoshi.model");
    ent->think = monster6_yoshi_think;
    ent->update = monster6_yoshi_update;
    ent->scale = vector3d(2,2,2);
    vector3d_copy(ent->position,position);
    return ent;
}

void monster6_yoshi_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    self->rotation.z += 0.01;
}

void monster6_yoshi_think(Entity *self)
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