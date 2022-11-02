#ifndef __MONSTER2_KONG_H__
#define __MONSTER2_KONG_H__

#include "entity.h"

/**
 * @brief Create a new monster2_kong entity
 * @param position where to spawn the kong at
 * @return NULL on error, or an monster2_kong entity pointer on success
 */
Entity *monster2_kong_new(Vector3D position);


#endif
