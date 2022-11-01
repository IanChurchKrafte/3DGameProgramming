#ifndef __MONSTER1_H__
#define __MONSTER1_H__

#include "entity.h"

/**
 * @brief Create a new monster1 entity
 * @param position where to spawn the monster1 at
 * @return NULL on error, or an monster1 entity pointer on success
 */
Entity *monster1_new(Vector3D position);

/**
 * @brief damage function for monster1 entity
 * @param damage the amount to change the health by
 * @param self the entity taking damage
 * @param heal 1 for healing, 0 for regular damage
 * @param inflictor the entity that is doing the damage
 * @return changes health in self->health
*/
void monster1_damage(int damage, Entity *self, int heal, Entity *inflictor);

/**
 * @brief when the entity dies, calls entity_free()
 * @param self the entity that is dead
*/
void monster1_onDeath(Entity *self);
#endif