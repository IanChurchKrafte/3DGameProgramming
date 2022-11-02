#ifndef __MONSTER7_CREEPER_H__
#define __MONSTER7_CREEPER_H__

#include "entity.h"

/**
 * @brief Create a new monster7_creeper entity
 * @param position where to spawn the monster7_creeper at
 * @return NULL on error, or an monster7_creeper entity pointer on success
 */
Entity *monster7_creeper_new(Vector3D position);


#endif
