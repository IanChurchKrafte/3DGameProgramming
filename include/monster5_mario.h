#ifndef __MONSTER5_MARIO_H__
#define __MONSTER5_MARIO_H__

#include "entity.h"

/**
 * @brief Create a new monster5_mario entity
 * @param position where to spawn the mario at
 * @return NULL on error, or an monster5_mario entity pointer on success
 */
Entity *monster5_mario_new(Vector3D position);


#endif
