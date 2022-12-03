
#include "simple_logger.h"
#include "defense3_turret1.h"
#include "gfc_types.h"


void defense3_turret1_update(Entity *self, Entity *player);

void defense3_turret1_think(Entity *self);

void defense3_turret1_BASE_upgrade(Entity *self);
void defense3_turret1_T1_upgrade(Entity *self);
void defense3_turret1_T2_upgrade(Entity *self);
void defense3_turret1_T3_upgrade(Entity *self);

Entity *defense3_turret1_new(Vector3D position, Vector3D rotation)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no defense3_turret1 for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/defense3_turret1.model");
    ent->think = defense3_turret1_think;
    ent->update = defense3_turret1_update;
    ent->scale = vector3d(0.05,0.05,0.05);
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
    ent->entityNum = 1;
    ent->state = ES_BASE;

    defense3_turret1_BASE_upgrade(ent);
    //ent->health = 300;
    //slog("%f", rotation.z);
    ent->rotation.z = rotation.z; //rotate 90 degrees so it spawns perpendicular to where the player is looking

    return ent;
    ent->selectedColor = gfc_color(1, 0.5, 0, 0.9);
}

void defense3_turret1_update(Entity *self, Entity *player)
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

void defense3_turret1_think(Entity *self)
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
                defense3_turret1_BASE_upgrade(self);
                self->stateSwitched = 0;
                break;
            case ES_T1:
                defense3_turret1_T1_upgrade(self);
                self->stateSwitched = 0; //so it can fail next check unless player wants to upgrade it
                break;
            case ES_T2:
                defense3_turret1_T2_upgrade(self);
                self->stateSwitched = 0;
                break;
            case ES_T3:
                defense3_turret1_T3_upgrade(self);
                self->stateSwitched = 0;
                break;
        }
    }
}

//for the fence and wall upgrading will only give more health for the enemies to break through
void defense3_turret1_BASE_upgrade(Entity *self){
    self->health = 200;
    self->attackDamage = 20;
    self->attackType = R_bullet;
    self->selectedColor = gfc_color(1,1,1,1);
    self->selected = 1;
}
void defense3_turret1_T1_upgrade(Entity *self){
    self->health = 200*2;
    self->attackDamage = 20 * 1.5;
    self->selectedColor = gfc_color(0, 1, 0, 0.8);
    self->selected = 1;
    slog("T1 upgrade");
}

void defense3_turret1_T2_upgrade(Entity *self){
    self->health = 200*3;
    self->attackDamage = self->attackDamage * 1.5;
    self->selectedColor = gfc_color(0, 0.5, 1, 0.8);
    self->selected = 1;
    slog("T2 upgrade");
}

void defense3_turret1_T3_upgrade(Entity *self){
    self->health = 200*4;
    self->attackDamage = self->attackDamage * 2.0;
    self->selectedColor = gfc_color(1, 0, 0, 0.8);
    self->selected = 1;
    slog("T3 upgrade");
}

/*eol@eof*/
