
#include "simple_logger.h"
#include "defense4_turret2.h"
#include "gfc_types.h"
#include "rayCast.h"


void defense4_turret2_update(Entity *self, Entity *player);

void defense4_turret2_think(Entity *self);

void defense4_turret2_BASE_upgrade(Entity *self);
void defense4_turret2_T1_upgrade(Entity *self);
void defense4_turret2_T2_upgrade(Entity *self);
void defense4_turret2_T3_upgrade(Entity *self);

Entity *defense4_turret2_new(Vector3D position, Vector3D rotation)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no defense4_turret2 for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/defense4_turret2.model");
    ent->think = defense4_turret2_think;
    ent->update = defense4_turret2_update;
    ent->scale = vector3d(2.5,2.5,2.5);
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
    ent->entityNum = 2;
    ent->state = ES_BASE;

    defense4_turret2_BASE_upgrade(ent);
    //ent->health = 200;
    //slog("%f", rotation.z);
    ent->rotation.z = rotation.z - (M_PI/2.0); //rotate 90 degrees so it spawns perpendicular to where the player is looking

    return ent;
    ent->selectedColor = gfc_color(1, 0.5, 0, 0.9);
}

void defense4_turret2_update(Entity *self, Entity *player)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);

    Entity *entityList[512];
    for(int i=0; i<512; i++)
        entityList[i] = player->entityList[i];
    
    vector3d_add(self->position,self->position,self->velocity);
    float distance = 0, temp = 1000;
    Entity *followedMonster;
    for(int i=0; i<512; i++){
        if(entityList[i] == NULL)
            continue; 
        
        if(entityList[i] && entityList[i]->type == ET_monster){
            distance = distance3D(self->position, entityList[i]->position);
            if (temp > distance){
                temp = distance;
                followedMonster = entityList[i];
            }
        }
    }

    if(followedMonster && distance <= 40){
        Vector2D facingVec;
        vector2d_sub(facingVec, followedMonster->position, self->position);
        float rotate = atan2(facingVec.y, facingVec.x);
        self->rotation.z = rotate + M_PI;
        slog("%f, %f, %f", self->position.x, self->position.y, self->position.z);
        Entity *hitEnt = rayCast(self, 50, player);
        if(hitEnt)
            entity_damage(player, hitEnt, 15, 0);
    }
    
    self->bounds.x = self->position.x;
    self->bounds.y = self->position.y;
    self->bounds.z = self->position.z;
    
}

void defense4_turret2_think(Entity *self)
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
                defense4_turret2_BASE_upgrade(self);
                self->stateSwitched = 0;
                break;
            case ES_T1:
                defense4_turret2_T1_upgrade(self);
                self->stateSwitched = 0; //so it can fail next check unless player wants to upgrade it
                break;
            case ES_T2:
                defense4_turret2_T2_upgrade(self);
                self->stateSwitched = 0;
                break;
            case ES_T3:
                defense4_turret2_T3_upgrade(self);
                self->stateSwitched = 0;
                break;
        }
    }
}

//for the fence and wall upgrading will only give more health for the enemies to break through
void defense4_turret2_BASE_upgrade(Entity *self){
    self->health = 300;
    self->attackDamage = 15;
    self->attackType = R_fire;
    self->selectedColor = gfc_color(1,1,1,1);
    self->selected = 1;
}
void defense4_turret2_T1_upgrade(Entity *self){
    self->health = 300*2;
    self->attackDamage = 15 * 1.5;
    self->selectedColor = gfc_color(0, 1, 0, 0.8);
    self->selected = 1;
    slog("T1 upgrade");
}

void defense4_turret2_T2_upgrade(Entity *self){
    self->health = 300*3;
    self->attackDamage = self->attackDamage * 1.5;
    self->selectedColor = gfc_color(0, 0.5, 1, 0.8);
    self->selected = 1;
    slog("T2 upgrade");
}

void defense4_turret2_T3_upgrade(Entity *self){
    self->health = 150*4;
    self->attackDamage = self->attackDamage * 2;
    self->selectedColor = gfc_color(1, 0, 0, 0.8);
    self->selected = 1;
    slog("T3 upgrade");
}

/*eol@eof*/
