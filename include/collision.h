#include "gfc_primitives.h"
#include "gfc_types.h"

/**
 * @brief check collision for the x when going in pos x
 * @param b the bounding box for the entity
 * @param plane the plane to check collision with
 * @return 0 for collision, 1 for no collision
 */
Uint8 collision_box_to_plane_x_pos(Box b, Plane3D plane);

/**
 * @brief check collision for the x when going in neg x
 * @param b the bounding box for the entity
 * @param plane the plane to check collision with
 * @return 0 for collision, 1 for no collision
 */
Uint8 collision_box_to_plane_x_neg(Box b, Plane3D plane);

/**
 * @brief check collision for the y
 * @param b the bounding box for the entity
 * @param plane the plane to check collision with
 * @return 0 for collision, 1 for no collision
 */
Uint8 collision_box_to_plane_y(Box b, Plane3D plane);

/**
 * @brief check collision for the z going up
 * @param b the bounding box for the entity
 * @param plane the plane to check collision with
 * @return 0 for collision, 1 for no collision
 */
Uint8 collision_box_to_plane_z_up(Box b, Plane3D plane);

/**
 * @brief check collision for the z going down
 * @param b the bounding box for the entity
 * @param plane the plane to check collision with
 * @return 0 for collision, 1 for no collision
 */
Uint8 collision_box_to_plane_z_down(Box b, Plane3D plane);