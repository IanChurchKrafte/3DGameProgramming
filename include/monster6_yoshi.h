#ifndef __MONSTER6_YOSHI_H__
#define __MONSTER6_YOSHI_H__

#include "entity.h"

/**
 * @brief Create a new monster6_yoshi entity
 * @param position where to spawn the yoshi at
 * @return NULL on error, or an monster6_yoshi entity pointer on success
 */
Entity *monster6_yoshi_new(Vector3D position);


#endif
