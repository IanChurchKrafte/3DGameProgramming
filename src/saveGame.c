#include "entity.h"
#include "simple_json.h"
#include "simple_json_object.h"
#include "simple_logger.h"
#include <stdio.h>

int fileExists(const char *fileName);

SJson * testSave(Entity *self, SJson *ent){
    sj_object_insert(ent, "entityType", sj_new_int(self->type));
    sj_object_insert(ent, "entityNum", sj_new_int(self->entityNum));

    sj_object_insert(ent, "position_x", sj_new_float(self->position.x));
    sj_object_insert(ent, "position_y", sj_new_float(self->position.y));
    sj_object_insert(ent, "position_z", sj_new_float(self->position.z));
            
    sj_object_insert(ent, "rotation_x", sj_new_float(self->rotation.x));
    sj_object_insert(ent, "rotation_y", sj_new_float(self->rotation.y));
    sj_object_insert(ent, "rotation_z", sj_new_float(self->rotation.z));

    sj_object_insert(ent, "health", sj_new_int(self->health));
    sj_object_insert(ent, "attackType", sj_new_int(self->attackType));
    sj_object_insert(ent, "state", sj_new_int(self->state));
    sj_object_insert(ent, "behindWall", sj_new_int(self->behindWall));

    sj_object_insert(ent, "isDescending", sj_new_int(self->isDescending));
    sj_object_insert(ent, "isJumping", sj_new_int(self->isJumping));
    sj_object_insert(ent, "startPosition", sj_new_float(self->startPosition));

    return ent;
}

void playerSave(Entity *player, SJson *ent){
    sj_object_insert(ent, "isPlayer", sj_new_int(player->isPlayer));

    sj_object_insert(ent, "position_x", sj_new_float(player->position.x));
    sj_object_insert(ent, "position_y", sj_new_float(player->position.y));
    sj_object_insert(ent, "position_z", sj_new_float(player->position.z));

    sj_object_insert(ent, "rotation_x", sj_new_float(player->rotation.x));
    sj_object_insert(ent, "rotation_y", sj_new_float(player->rotation.y));
    sj_object_insert(ent, "rotation_z", sj_new_float(player->rotation.z));

    sj_object_insert(ent, "health", sj_new_int(player->health));
    sj_object_insert(ent, "attackType", sj_new_int(player->attackType));
    sj_object_insert(ent, "points", sj_new_int(player->points));
    
    sj_object_insert(ent, "isJumping", sj_new_int(player->isJumping));
    sj_object_insert(ent, "isDescending", sj_new_int(player->isDescending));
    sj_object_insert(ent, "startPosition", sj_new_float(player->startPosition));
}

void playerLoad(Entity *player, SJson *ent){
    sj_get_float_value(sj_object_get_value(ent, "position_x"), &player->position.x);
    sj_get_float_value(sj_object_get_value(ent, "position_y"), &player->position.y);
    sj_get_float_value(sj_object_get_value(ent, "position_z"), &player->position.z);

    sj_get_float_value(sj_object_get_value(ent, "rotation_x"), &player->rotation.x);
    sj_get_float_value(sj_object_get_value(ent, "rotation_y"), &player->rotation.y);
    sj_get_float_value(sj_object_get_value(ent, "rotation_z"), &player->rotation.z);

    sj_get_integer_value(sj_object_get_value(ent, "health"), &player->health);
    sj_get_integer_value(sj_object_get_value(ent, "attackType"), &player->attackType);
    sj_get_integer_value(sj_object_get_value(ent, "points"), &player->points);

    sj_get_integer_value(sj_object_get_value(ent, "isDescending"), &player->isDescending);
    sj_get_integer_value(sj_object_get_value(ent, "isJumping"), &player->isJumping);
    sj_get_float_value(sj_object_get_value(ent, "startPosition"), &player->startPosition);
}
Entity * testLoad(Entity *self, SJson *ent){
    sj_get_integer_value(sj_object_get_value(ent, "health"), &self->health);
    sj_get_integer_value(sj_object_get_value(ent, "attackType"), &self->attackType);
    sj_get_integer_value(sj_object_get_value(ent, "state"), &self->state);
    sj_get_integer_value(sj_object_get_value(ent, "behindWall"), &self->behindWall);

    sj_get_integer_value(sj_object_get_value(ent, "isDescending"), &self->isDescending);
    sj_get_integer_value(sj_object_get_value(ent, "isJumping"), &self->isJumping);
    sj_get_float_value(sj_object_get_value(ent, "startPosition"), &self->startPosition);

    return self;
    
}



int saveGame_to_json(Entity *self, const char *fileName){
    if (!self)return 0;
    if(fileExists(fileName) == 0)return 0;
   
    SJson *savedJson = sj_load("saves/saveTest.json");
    SJson *json = sj_object_new();
    SJson *position = sj_array_new();
    SJson *rotation = sj_array_new();
    //SJson *jumpStatus = sj_array_new();

    sj_object_insert(json, "health", sj_new_int(self->health));
    sj_object_insert(json, "attackType", sj_new_int(self->attackType));
    sj_object_insert(json, "state", sj_new_int(self->state));
    sj_object_insert(json, "type", sj_new_int(self->type));
    sj_object_insert(json, "entityNum", sj_new_int(self->entityNum));
    sj_object_insert(json, "behindWall", sj_new_int(self->behindWall));

    
    sj_object_insert(json, "isDescending", sj_new_int(self->isDescending));
    sj_object_insert(json, "isJumping", sj_new_int(self->isJumping));
    sj_object_insert(json, "startPosition", sj_new_float(self->startPosition));

    // sj_object_insert(json, "position_x", sj_new_float(self->position.x));
    // sj_object_insert(json, "position_y", sj_new_float(self->position.y));
    // sj_object_insert(json, "position_z", sj_new_float(self->position.z));

    // sj_object_insert(json, "rotation_x", sj_new_float(self->rotation.x));
    // sj_object_insert(json, "rotation_y", sj_new_float(self->rotation.y));
    // sj_object_insert(json, "rotation_z", sj_new_float(self->rotation.z));

    // slog("starting position");
    // sj_array_append(position, sj_new_float(self->position.x));
    // sj_array_append(position, sj_new_float(self->position.y));
    // sj_array_append(position, sj_new_float(self->position.z));
    // sj_object_insert(json, "position", position);
    // //sj_free(position);
    // slog("end position");

    // slog("starting rotation");
    // sj_array_append(rotation, sj_new_float(self->rotation.x));
    // sj_array_append(rotation, sj_new_float(self->rotation.y));
    // sj_array_append(rotation, sj_new_float(self->rotation.z));
    // sj_object_insert(json, "rotation", rotation);
    // //sj_free(rotation);
    // slog("ending rotation");

    sj_save(json, fileName);
    
    sj_free(json);
    return 1;
}

int loadGame_from_json(Entity *self, const char *fileName){
    if(!self)return 0;
    if(fileExists(fileName) == 0)return 0;

    SJson *json = sj_object_new();
    json = sj_load("saves/saveTest.json");
    sj_get_integer_value(sj_object_get_value(json, "playerHealth"), &self->health);
    sj_get_integer_value(sj_object_get_value(json, "attackType"), &self->attackType);
    sj_get_integer_value(sj_object_get_value(json, "state"), &self->state);
    sj_get_integer_value(sj_object_get_value(json, "type"), &self->type);
    sj_get_integer_value(sj_object_get_value(json, "entityNum"), &self->entityNum);
    sj_get_integer_value(sj_object_get_value(json, "behindWall"), &self->behindWall);
    
    sj_get_integer_value(sj_object_get_value(json, "isJumping"), &self->isJumping);
    sj_get_integer_value(sj_object_get_value(json, "isDescending"), &self->isDescending);
    sj_get_float_value(sj_object_get_value(json, "startPosition"), &self->startPosition);

    sj_get_float_value(sj_object_get_value(json, "position_x"), &self->position.x);
    sj_get_float_value(sj_object_get_value(json, "position_y"), &self->position.y);
    sj_get_float_value(sj_object_get_value(json, "position_z"), &self->position.z);

    sj_get_float_value(sj_object_get_value(json, "rotation_x"), &self->rotation.x);
    sj_get_float_value(sj_object_get_value(json, "rotation_y"), &self->rotation.y);
    sj_get_float_value(sj_object_get_value(json, "rotation_z"), &self->rotation.z);

    // SJson *position, *rotation;
    // Vector3D pos, rot;
    // position = sj_object_get_value(json, "position");
    // rotation = sj_object_get_value(json, "rotation");
    // for(int i=0; i<2; i++){
    //     switch (i){
    //         case 0:
    //             sj_get_float_value(sj_array_get_nth(position, i), &self->position.x);
    //             sj_get_float_value(sj_array_get_nth(rotation, i), &self->rotation.x);
    //             break;
    //         case 1:
    //             sj_get_float_value(sj_array_get_nth(position, i), &self->position.y);
    //             sj_get_float_value(sj_array_get_nth(rotation, i), &self->rotation.y);
    //             break;
    //         case 2:
    //             sj_get_float_value(sj_array_get_nth(position, i), &self->position.z);
    //             sj_get_float_value(sj_array_get_nth(rotation, i), &self->rotation.z);
    //             break;
    //     }

    // }
    // sj_free(position);
    // sj_free(rotation);

    sj_free(json);
    return 1;

}

int fileExists(const char *fileName){
    FILE *fp = fopen(fileName, "r");
    if(fp != NULL)return 1;
    else return 0;
}