
#include "simple_logger.h"
#include "monster4_skelly.h"
#include "collision.h"


void monster4_skelly_update(Entity *self, Entity *player);

void monster4_skelly_think(Entity *self);

Model *idleAnim[9];

Entity *monster4_skelly_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no monster2_kong for you!");
        return NULL;
    }
    //ent->selectedColor = gfc_color(0.1,1,0.1,1);
    //ent->color = gfc_color(1,1,1,1);

    idleAnim[0] = gf3d_model_load("models/monster4_skelleton.model");
    idleAnim[1] = gf3d_model_load("models/skelly/skelly_walkAnim1.model");
    idleAnim[2] = gf3d_model_load("models/skelly/skelly_walkAnim2.model");
    idleAnim[3] = gf3d_model_load("models/skelly/skelly_walkAnim3.model");
    idleAnim[4] = gf3d_model_load("models/skelly/skelly_walkAnim4.model");
    idleAnim[5] = gf3d_model_load("models/skelly/skelly_walkAnim5.model");
    idleAnim[6] = gf3d_model_load("models/skelly/skelly_walkAnim6.model");
    idleAnim[7] = gf3d_model_load("models/skelly/skelly_walkAnim7.model");
    idleAnim[8] = gf3d_model_load("models/skelly/skelly_walkAnim8.model");
    // idleAnim[9] = gf3d_model_load("models/skelly/skelly_walkAnim9.model");
    // idleAnim[10] = gf3d_model_load("models/skelly/skelly_walkAnim10.model");

    //idleAnim[1] = gf3d_model_load("monster4_skelly_anim.model");


    ent->model = idleAnim[0];
    ent->think = monster4_skelly_think;
    ent->update = monster4_skelly_update;
    ent->scale = vector3d(3,3,3);
    ent->damage = entity_damage;
    ent->onDeath = entity_onDeath;

    vector3d_copy(ent->position,position);

    //ent->position.z -= 5;
    ent->bounds.x = position.x;
    ent->bounds.y = position.y;
    ent->bounds.z = position.z;
    ent->bounds.w = 10;
    ent->bounds.h = 10;
    ent->bounds.d = 10;

    ent->health = 100;

    ent->state = ES_idle;

    ent->type = ET_monster;
    ent->entityNum = 4;
    return ent;
}
int lastCheck = 0;
void monster4_skelly_update(Entity *self, Entity *player)
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

    //rotate towards player
    // Vector2D facingVec;
    // vector2d_sub(facingVec, player->position, self->position);
    // float rotate = atan2(facingVec.y, facingVec.x);
    // self->rotation.z = rotate + M_PI;

    Vector2D *selfPos = NULL;
    Vector2D vect = vector2d(self->position.x, self->position.y);
    selfPos = &vect;
    Vector2D playerPos = vector2d(player->position.x, player->position.y);
    if(!gfc_box_overlap(player->bounds, self->bounds)){
        self->state = ES_hunt;
        //slog("not colliding with player");
        //add entity list for entities spawn in by the player
        if(player->defenseCount == 0){
            //slog("no defenses found");
            vector2d_move_towards(selfPos, vector2d(self->position.x, self->position.y), playerPos, 0.5);
            self->position.x = selfPos->x;
            self->position.y = selfPos->y;
        }
        for(int i=0; i<player->defenseCount; i++){
            //slog("knows that a fence is here");
            if(!gfc_box_overlap(player->defenseBounds[i], self->bounds)){
                //slog("not colliding with defense entity");
                vector2d_move_towards(selfPos, vector2d(self->position.x, self->position.y), playerPos, 0.5);
                self->position.x = selfPos->x;
                self->position.y = selfPos->y;
                //return 1;
            }
            else{
                //slog("stuck beind wall, now damaging it");
                self->behindWall = 1;
            }
        }
    }
    else{
        self->state = ES_idle;
    }


    // Box centerBox = gfc_box(0,0,-25, 10,10,10);
    // Plane3D bottomPlane = gfc_plane3d(0,0,-25,25);
    // if(collision_box_to_plane_z_down(self->bounds, bottomPlane) && !gfc_box_overlap(self->bounds, centerBox)){ //check for collison on gound and center box
    //     self->position.z -=1;
    // }
    
    


    // else if(lastCheck == 9){
    //     self->model = idleAnim[9];
    //     lastCheck = 10;
    // }
    // else if(lastCheck == 10){
    //     self->model = idleAnim[10];
    //     lastCheck = 0;
    // }

    //self->rotation.z += 0.01;
}

void monster4_skelly_think(Entity *self)
{
    if (!self)return;
    switch(self->state)
    {
        case ES_idle:
            //look for player
            //do idle animation
            self->model = idleAnim[0];
            break;
        case ES_hunt:
            // set move towards player
            if(self->isDead == 1)
                self->state = ES_dead;
            if(lastCheck == 0){
                self->model = idleAnim[0];
                lastCheck = 1;
            }
            else if(lastCheck == 1){
                self->model = idleAnim[1];
                lastCheck = 2;
            }
            else if(lastCheck == 2){
                self->model = idleAnim[2];
                lastCheck = 3;
            }
            else if(lastCheck == 3){
                self->model = idleAnim[3];
                lastCheck = 4;
            }
            else if(lastCheck == 4){
                self->model = idleAnim[4];
                lastCheck = 5;
            }
            else if(lastCheck == 5){
                self->model = idleAnim[5];
                lastCheck = 6;
            }
            else if(lastCheck == 6){
                self->model = idleAnim[6];
                lastCheck = 7;
            }
            else if(lastCheck == 7){
                self->model = idleAnim[7];
                lastCheck = 8;
            }
            else if(lastCheck == 8){
                self->model = idleAnim[8];
                lastCheck = 0;
            }
            break;
        case ES_dead:
            // remove myself from the system
            break;
        case ES_attack:
            // run through attack animation / deal damage
            break;
        case ES_T1:
            break;
        case ES_T2:
            break;
        case ES_T3:
            break;
        case ES_BASE:
            break;
    }
}

/*eol@eof*/
