#ifndef __GUNMODEL_H__
#define __GUNMODEL_H__

#include "entity.h"

/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity *gunModel_new(Vector3D position);

void gunModel_update(Entity *self, Entity *player);


#endif
