#ifndef __SLOWGOO_H__
#define __SLOWGOO_H__

#include "entity.h"

/**
 * @brief spawn goo that slows entities inside of it
 * @param position where to spawn the goo
 * @return NULL on error, or an entity pointer to the goo splatter
*/

Entity *slowGoo_new(Vector3D position);

#endif