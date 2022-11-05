#ifndef __DEFENSE2_SMALLWALL_H__
#define __DEFENSE2_SMALLWALL_H__

#include "entity.h"

/**
 * @brief Create a new defense2_smallWall entity
 * @param position where to spawn a small wall at
 * @return NULL on error, or an defense2_smallWall entity pointer on success
 */
Entity *defense2_smallWall_new(Vector3D position, Vector3D rotation);


#endif
