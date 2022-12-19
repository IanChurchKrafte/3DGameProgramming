
#include "simple_logger.h"
#include "gunModel.h"


void gunModel_update(Entity *self, Entity *player);

void gunModel_think(Entity *self, Entity *player);

Entity *gunModel_new(Vector3D position)
{
    //takes in the player position, then moved into the right spot and will sit infront of the player
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no agumon for you!");
        return NULL;
    }
    //ent->position.x += 5;
    ent->selectedColor = gfc_color(0.1,1,0.1,1);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/player.model");
    ent->think = gunModel_think;
    ent->update = gunModel_update;
    ent->scale = vector3d(1,1,1);
    vector3d_copy(ent->position,position);
    ent->position.x += 2;
    return ent;
}

void gunModel_update(Entity *self, Entity *player)
{
    if (!self || !player)
    {
        slog("self pointer not provided");
        return;
    }

    self->position = player->position;
    self->position.z -= 0.15;

    self->rotation.y = player->rotation.x;
    self->rotation.y += M_PI;
    self->rotation.z = player->rotation.z;
    self->rotation.z += M_PI_2;
}

void gunModel_think(Entity *self, Entity *player)
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
