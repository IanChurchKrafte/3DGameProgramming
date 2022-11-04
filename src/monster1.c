/*
charmander model
called monster1
this is the first enemy
*/

#include "simple_logger.h"
#include "monster1.h"
#include "gfc_vector.h"
#include "gfc_primitives.h"
#include "stdlib.h"

#include "collision.h"

void monster1_update(Entity *self);
void monster1_think(Entity *self);
void monster1_damage(int damage, Entity *self, int heal, Entity *inflictor);
void monster1_onDeath(Entity *self);

//Box bottom = gfc_box(0,0,-30,10,10,10);
//Plane3D bottom = gfc_plane3d(0, 0, -30, 30);
//Vector3D bottom = (0,0,-30);
//Vector3D bottom = vector3d(0.0,0.0,-30.0);

Entity *monster1_new(Vector3D position){
    Entity *ent = NULL;

    ent = entity_new();
    if(!ent){
        slog("cant spawn monster1");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);
    ent->model=gf3d_model_load("models/monster1_charmander.model");
    //Vector3D scale = (5.0,5.0,5.0);
    ent->scale = vector3d(2,2,2);
    ent->think = monster1_think;
    ent->update = monster1_update;
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;//monster1_onDeath;
    //ent->rotation.x+=1.5;
    vector3d_copy(ent->position,position);
    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.w = ent->scale.x+10;
    ent->bounds.h = ent->scale.z+10;
    ent->bounds.d = ent->scale.y+10;

    ent->health = 100;

    return ent;
}

void monster1_update(Entity *self){
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);

    self->bounds.x = self->position.x;
    self->bounds.y = self->position.y;
    self->bounds.z = self->position.z;
    self->bounds.w = 10;
    self->bounds.h = 10;
    self->bounds.d = 10;
    // = gfc_box(self->position.x, self->position.y, self->position.z, self->scale.x, self->scale.y, self->scale.z);
}

void monster1_think(Entity *self){
    if(!self){
        slog("self pointer not provided");
        return;
    }
    int num = (rand() % 6);
    //move in the x
    if(num == 10){
        if(collision_box_to_plane_x_pos(self->bounds, gfc_plane3d(40,0,0,40))){
            self->position.x+=10;
            //slog("x: %f, y: %f, z: %f, w: %f, h: %f, d: %f", self->bounds.x, self->bounds.y, self->bounds.z, self->bounds.w, self->bounds.h, self->bounds.d);
        }
    }
    if(num == 11){
        if(collision_box_to_plane_x_neg(self->bounds, gfc_plane3d(-40,0,0,40))){
            self->position.x-=10;
            //slog("position: x: %f", self->position.x);
            //slog("x: %f, y: %f, z: %f, w: %f, h: %f, d: %f", self->bounds.x, self->bounds.y, self->bounds.z, self->bounds.w, self->bounds.h, self->bounds.d);
        }
    }

    //move in the y
    if(num == 12){//pos
        if(collision_box_to_plane_y(self->bounds, gfc_plane3d(0,40,0,40))){
            self->position.y+=10;
        }
    }
    if(num == 13){//neg
        if(collision_box_to_plane_y(self->bounds, gfc_plane3d(0,-40,0,40))){
            self->position.y-=10;
        }
    }

    //move in the z
    if(num == 14){
        //slog("bounds: %f, %f, %f", self->bounds.x, self->bounds.y, self->bounds.z);
        if(collision_box_to_plane_z_up(self->bounds, gfc_plane3d(0,0,10,10))){
            self->position.z+=10;
        }
    }
    if(num == 15){
        //if(!gfc_box_overlap(self->bounds, gfc_box(0,0,-30,10,10,10))){
        if(collision_box_to_plane_z_down(self->bounds, gfc_plane3d(0,0,-30,30))){
            self->position.z-=10;
            //slog("x: %f, y: %f, z: %f, w: %f, h: %f, d: %f", self->bounds.x, self->bounds.y, self->bounds.z, self->bounds.w, self->bounds.h, self->bounds.d);
        }
    }

    //self->rotation.x += -0.005;
}

void monster1_damage(int damage, Entity *self, int heal, Entity *inflictor){
    if(heal == 0){//damage not heal
        int temp = self->health - damage;
        //slog("temp: %i, health: %i, damage: %i", temp, self->health, damage);
        if(temp<0) temp = 0;
        if(temp == 0){
            self->onDeath(self);
            inflictor->points+=50;
        }
        else{
            inflictor->points += 10;
            slog("temp: %i",temp);
            self->health = temp;
        }
        
    }
    else{//heal monster1 with number from damage value
        int temp = self->health + damage;
        if(temp>100) temp = 100;
        self->health = temp;
    }
    
}

void monster1_onDeath(Entity *self){
    slog("monster1 dead :(");
    entity_free(self);
}

// bool Monster1_collisionDetection(Entity *self){
//     // float x = self->position.x;
//     // float y = self->position.y;
//     // float z = self->position.z;


// }

