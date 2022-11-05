#ifndef __DEFENSE1_SMALLFENCE_H__
#define __DEFENSE1_SMALLFENCE_H__

#include "entity.h"

/**
 * @brief Create a new defense1_smallFence entity
 * @param position where to spawn a small fence at
 * @return NULL on error, or an defense1_smallFence entity pointer on success
 */
Entity *defense1_smallFence_new(Vector3D position, Vector3D rotation);


#endif
