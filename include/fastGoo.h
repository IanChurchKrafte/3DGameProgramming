#ifndef __FASTGOO_H__
#define __FASTGOO_H__

#include "entity.h"

/**
 * @brief spawn goo that makes entities faster inside of it
 * @param position where to spawn the goo
 * @return NULL on error, or an entity pointer to the goo splatter
*/

Entity *fastGoo_new(Vector3D position);

#endif