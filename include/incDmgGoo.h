#ifndef __INCDMGGOO_H__
#define __INCDMGGOO_H__

#include "entity.h"

/**
 * @brief spawn goo that increases damage for the player inside of it
 * @param position where to spawn the goo
 * @return NULL on error, or an entity pointer to the goo splatter
*/

Entity *incDmgGoo_new(Vector3D position);

#endif