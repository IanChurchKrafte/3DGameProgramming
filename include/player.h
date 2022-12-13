#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/**
 * @brief Create a new player entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an player entity pointer on success
 */
Entity *player_new(Vector3D position);
int getPoints();

void save();

void loadEntities(Entity *self);

void player_spawnWaves(); 

void player_damage(int damage, Entity *self, int heal, Entity *inflictor);

#endif
