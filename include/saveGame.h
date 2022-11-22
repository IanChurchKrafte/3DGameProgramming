#ifndef __SAVEGAME_H__
#define __SAVEGAME_H__

#include "entity.h"
#include "simple_json.h"
#include "simple_json_object.h"

/**
 * @brief save given entity into json
 * @param entity the entity to save
 * @param fileName destination to save too
 * @return returns 1 on success, NULL on fail
*/
int saveGame_to_json(Entity *self, const char *fileName);

/**
 * @brief load info from json into an entity
 * @param entity the entity to load into
 * @param fileName the json file to load from
 * @returns NULL on failure, entity on success
*/
int loadGame_from_json(Entity *self, const char *fileName);

SJson * testSave(Entity *self, SJson *ent);
Entity * testLoad(Entity *self, SJson *ent);
void playerSave(Entity *player, SJson *ent);
void playerLoad(Entity *player, SJson *ent);

#endif