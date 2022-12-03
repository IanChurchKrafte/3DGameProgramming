
#include "simple_logger.h"
#include "defense5_turret3.h"
#include "gfc_types.h"


void defense5_turret3_update(Entity *self, Entity *player);

void defense5_turret3_think(Entity *self);

void defense5_turret3_BASE_upgrade(Entity *self);
void defense5_turret3_T1_upgrade(Entity *self);
void defense5_turret3_T2_upgrade(Entity *self);
void defense5_turret3_T3_upgrade(Entity *self);

Entity *defense5_turret3_new(Vector3D position, Vector3D rotation)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no defense5_turret3 for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/defense5_turret3.model");
    ent->think = defense5_turret3_think;
    ent->update = defense5_turret3_update;
    ent->scale = vector3d(1,1,1);
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;

    vector3d_copy(ent->position,position);

    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.w = 10;
    ent->bounds.h = 10;
    ent->bounds.d = 5;

    ent->type = ET_turret;
    ent->entityNum = 3;
    ent->state = ES_BASE;
    
    defense5_turret3_BASE_upgrade(ent);
    //ent->health = 100;
    //slog("%f", rotation.z);
    ent->rotation.z = rotation.z + (M_PI); //rotate 90 degrees so it spawns perpendicular to where the player is looking
    //ent->rotation.y = rotation.y;
    return ent;
    ent->selectedColor = gfc_color(1, 0.5, 0, 0.9);
}

void defense5_turret3_update(Entity *self, Entity *player)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    
    self->bounds.x = self->position.x;
    self->bounds.y = self->position.y;
    self->bounds.z = self->position.z;
    
}

void defense5_turret3_think(Entity *self)
{
    if (!self)return;
    if(self->stateSwitched){
        switch(self->state)
        {
            case ES_idle:
                break;
            case ES_hunt:
                break;
            case ES_dead:
                break;
            case ES_attack:
                break;

            case ES_BASE:
                defense5_turret3_BASE_upgrade(self);
                self->stateSwitched = 0;
                break;
            case ES_T1:
                defense5_turret3_T1_upgrade(self);
                self->stateSwitched = 0; //so it can fail next check unless player wants to upgrade it
                break;
            case ES_T2:
                defense5_turret3_T2_upgrade(self);
                self->stateSwitched = 0;
                break;
            case ES_T3:
                defense5_turret3_T3_upgrade(self);
                self->stateSwitched = 0;
                break;
        }
    }
}

//for the fence and wall upgrading will only give more health for the enemies to break through
void defense5_turret3_BASE_upgrade(Entity *self){
    self->health = 100;
    self->attackDamage = 10;
    self->attackType = R_bullet;
    self->selectedColor = gfc_color(1,1,1,1);
    self->selected = 1;
}
void defense5_turret3_T1_upgrade(Entity *self){
    self->health = 100*2;
    self->attackDamage = 10 * 1.5;
    self->selectedColor = gfc_color(0, 1, 0, 0.8);
    self->selected = 1;
    slog("T1 upgrade");
}

void defense5_turret3_T2_upgrade(Entity *self){
    self->health = 100*3;
    self->attackDamage = self->attackDamage * 1.5;
    self->selectedColor = gfc_color(0, 0.5, 1, 0.8);
    self->selected = 1;
    slog("T2 upgrade");
}

void defense5_turret3_T3_upgrade(Entity *self){
    self->health = 100*4;
    self->attackDamage = self->attackDamage * 2;
    self->selectedColor = gfc_color(1, 0, 0, 0.8);
    self->selected = 1;
    slog("T3 upgrade");
}

/*eol@eof*/
