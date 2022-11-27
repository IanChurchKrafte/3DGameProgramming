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

/**
 * @brief save an entity into a json obj
 * @param self the entity to save
 * @param ent the json obj to save it into
 * @return json object containing the info
*/
SJson * testSave(Entity *self, SJson *ent);

/**
 * @brief load and entity from a json
 * @param self entity to load info into
 * @param ent json obj to load from
 * @return entity struct
*/
Entity * testLoad(Entity *self, SJson *ent);

/**
 * @brief save player from json
*/
void playerSave(Entity *player, SJson *ent);

/**
 * @brief load player from json
*/
void playerLoad(Entity *player, SJson *ent);

#endif