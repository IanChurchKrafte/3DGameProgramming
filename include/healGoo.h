#ifndef __HEALGOO_H__
#define __HEALGOO_H__

#include "entity.h"

/**
 * @brief spawn goo that heals the player inside of it
 * @param position where to spawn the goo
 * @return NULL on error, or an entity pointer to the goo splatter
*/

Entity *healGoo_new(Vector3D position);

#endif