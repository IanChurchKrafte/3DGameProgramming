#ifndef __DEFENSE4_TURRET2_H__
#define __DEFENSE4_TURRET2_H__

#include "entity.h"

/**
 * @brief Create a new defense4_turret2 entity
 * @param position where to spawn a turret2 at
 * @return NULL on error, or an defense4_turret2 entity pointer on success
 */
Entity *defense4_turret2_new(Vector3D position, Vector3D rotation);


#endif
