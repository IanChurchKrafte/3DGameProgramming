
#include "simple_logger.h"
#include "gunModel.h"


void gunModel_update(Entity *self, Entity *player);

void gunModel_think(Entity *self, Entity *player);

Model *guns[5];

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
    guns[0] = gf3d_model_load("models/player.model");
    guns[1] = gf3d_model_load("models/pistol.model");
    guns[2] = gf3d_model_load("models/ak47.model");
    guns[3] = gf3d_model_load("models/awp.model");
    guns[4] = gf3d_model_load("models/shotgun.model");

    ent->selectedColor = gfc_color(0.1,1,0.1,1);
    ent->color = gfc_color(1,1,1,1);
    ent->model = guns[0];
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

    
    Vector3D defaultRot = {0};
    switch(player->attackType){
        case R_bullet:
            //PISTOL WITH HANDS
            self->model = guns[0];
            self->scale = vector3d(1,1,1);

            self->position = player->position;
            self->position.z -= 0.15;

            self->rotation = defaultRot;
            self->rotation.y = player->rotation.x;
            self->rotation.y += M_PI;
            self->rotation.z = player->rotation.z;
            self->rotation.z += M_PI_2;
            break;
        case R_fire:
            //REVOLVER
            self->model = guns[1];
            self->scale = vector3d(1,1,1);

            self->position = player->position;
            self->position.z -= 0.25;
            self->position.x += 0.1;

            self->rotation = defaultRot;
            self->rotation.x = (-1) * player->rotation.x;
            self->rotation.x -= M_PI;
            self->rotation.z = player->rotation.z;
            break;
        case R_ice:
            //AK
            self->model = guns[2];
            self->scale = vector3d(0.01,0.01,0.01);

            self->position = player->position;
            self->position.z -= 0.5;

            self->rotation = defaultRot;
            self->rotation.y = -M_PI;
            self->rotation.x = player->rotation.x;
            self->rotation.x += M_PI;
            self->rotation.z = player->rotation.z;
            break;
        case R_magic:
            //AWP
            self->model = guns[3];
            self->scale = vector3d(0.1, 0.1, 0.1);

            self->position = player->position;
            self->position.z -= 1.5;
            self->position.y += 0.5;
            self->position.x += 0.5;

            self->rotation = defaultRot;
            self->rotation.y = (-1) * player->rotation.x;
            self->rotation.y += M_PI_2;
            self->rotation.z = player->rotation.z;
            self->rotation.z -= M_PI_2;
            break;
        case R_melee:
            //SHOTGUN
            self->model = guns[4];
            self->scale = vector3d(0.1, 0.1, 0.1);

            self->position = player->position;
            self->position.z -= 1.75;
            self->position.y += 0.5;
            self->position.x += 0.5;
            
            self->rotation = defaultRot;
            self->rotation.y = (-1) * player->rotation.x;
            self->rotation.y += M_PI;
            self->rotation.z = player->rotation.z;
            self->rotation.z -= M_PI_2;
            break;
    }
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
