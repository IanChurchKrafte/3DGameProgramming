#ifndef __MONSTER3_PORYGON_H__
#define __MONSTER3_PORYGON_H__

#include "entity.h"

/**
 * @brief Create a new monster3_porygon entity
 * @param position where to spawn the porygon at
 * @return NULL on error, or an monster3_porygon entity pointer on success
 */
Entity *monster3_porygon_new(Vector3D position);


#endif
