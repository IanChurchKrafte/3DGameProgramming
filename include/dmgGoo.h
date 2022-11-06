#ifndef __DMGGOO_H__
#define __DMGGOO_H__

#include "entity.h"

/**
 * @brief spawn goo that damages entities inside of it
 * @param position where to spawn the goo
 * @return NULL on error, or an entity pointer to the goo splatter
*/

Entity *dmgGoo_new(Vector3D position);

#endif