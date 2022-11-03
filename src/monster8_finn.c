
#include "simple_logger.h"
#include "monster8_finn.h"


void monster8_finn_update(Entity *self);

void monster8_finn_think(Entity *self);

Entity *monster8_finn_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no monster8_finn for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/monster8_finn.model");
    ent->think = monster8_finn_think;
    ent->update = monster8_finn_update;
    ent->scale = vector3d(0.1,0.1,0.1);
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

    ent->resistance = 4; //resistant to ice damage
    ent->selectedColor = gfc_color(0,0,1,0.9);//blue 
    return ent;
}

void monster8_finn_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    self->rotation.z += 0.01;
}

void monster8_finn_think(Entity *self)
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
