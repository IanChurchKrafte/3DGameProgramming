#ifndef __MONSTER1_H__
#define __MONSTER1_H__

#include "entity.h"

/**
 * @brief Create a new monster1 entity
 * @param position where to spawn the monster1 at
 * @return NULL on error, or an monster1 entity pointer on success
 */
Entity *monster1_new(Vector3D position);


#endif