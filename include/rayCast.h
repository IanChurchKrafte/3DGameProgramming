#ifndef __RAYCAST_H__
#define __RAYCAST_H__

#include "entity.h"
#include "simple_logger.h"

/**
 * @brief do a ray cast with a given distance
 * @param self, the gun you want to raycast from
 * @param distance, the distance to draw the line segment for
 * @param entityList, the list of spawned entities
 * @param i, the count of spawned entities.
 * @return entity that gets ray casted
*/
Entity * rayCast(Entity *self, float distance, Entity *player);

/**
 * @brief distance formula for 3d 
 * @param start, starting point
 * @param end, ending point
 * @return distance in float
*/
float distance3D(Vector3D start, Vector3D end);


float edge3DLength(Edge3D e);
#endif