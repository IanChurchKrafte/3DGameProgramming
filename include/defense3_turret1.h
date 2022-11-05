#ifndef __DEFENSE3_TURRET1_H__
#define __DEFENSE3_TURRET1_H__

#include "entity.h"

/**
 * @brief Create a new defense3_turret1 entity
 * @param position where to spawn a turret1 at
 * @return NULL on error, or an defense3_turret1 entity pointer on success
 */
Entity *defense3_turret1_new(Vector3D position, Vector3D rotation);


#endif
