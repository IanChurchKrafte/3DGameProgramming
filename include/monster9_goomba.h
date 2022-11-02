#ifndef __MONSTER9_GOOMBA_H__
#define __MONSTER9_GOOMBA_H__

#include "entity.h"

/**
 * @brief Create a new monster9_goomba entity
 * @param position where to spawn the monster9_goomba at
 * @return NULL on error, or an monster9_goomba entity pointer on success
 */
Entity *monster9_goomba_new(Vector3D position);


#endif
