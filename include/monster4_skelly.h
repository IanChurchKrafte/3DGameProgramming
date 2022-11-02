#ifndef __MONSTER4_SKELLY_H__
#define __MONSTER4_SKELLY_H__

#include "entity.h"

/**
 * @brief Create a new monster4_skelly entity
 * @param position where to spawn the skelleton at
 * @return NULL on error, or an monster4_skelly entity pointer on success
 */
Entity *monster4_skelly_new(Vector3D position);


#endif
