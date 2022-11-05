#include "gfc_vector.h"
#include "gfc_primitives.h"
#include "gfc_types.h"
#include "simple_logger.h"

#include <stdlib.h>
//#include <stdbool.h>

//return 0 if there is collision, 1 if not;
Uint8 collision_box_to_plane_x_pos(Box b, Plane3D plane){
    float distance;
    if(b.x < 0){
        distance = abs(b.x) - abs(plane.x);
        //slog("distance: %f", distance);
    }
    else{
        distance = plane.x - b.x;
        //slog("b.x: %f, distance: %f", b.x, distance);
    }
    if(distance <= 1.0){
        slog("hit collision in x! distance: %f", distance);
        return 0;
    }
    else{
        //slog("plane.x: %f",plane.x);
        //slog("distance: %f", distance);
        return 1;
    }
}

Uint8 collision_box_to_plane_x_neg(Box b, Plane3D plane){
    float distance;
    if(b.x >= 0){
        distance = abs(plane.x) - b.x;
        //slog("distance: %f", distance);
    }
    else{
        distance = abs(plane.x) - abs(b.x);
        //slog("distance: %f", distance);
    }

    if(distance <= 1.0){
        slog("hit collision in x_neg! distance: %f", distance);
        return 0;
    }
    else{
        return 1;
    }
}

Uint8 collision_box_to_plane_y(Box b, Plane3D plane){
    float distance;
    if(b.y < 0){
        distance = abs(b.y) - abs(plane.y);
    }
    else{
        distance = b.y - plane.y;
    }
    if(abs(distance) <= 1.0){
        slog("hit collision in y! distance: %f", distance);
        return 0;
    }
    else{
        //slog("plane.y: %f",plane.y);
        //slog("distance: %f", distance);
        return 1;
    }
}

Uint8 collision_box_to_plane_z_up(Box b, Plane3D plane){
    float distance;
    if(b.z < 0){
        distance = (abs(b.z) + b.h) - abs(plane.y);
    }
    else{
        distance = (b.z + b.h) - plane.z;
    }
    if(abs(distance) <= 1.0){
        slog("hit collision in z_up! distance: %f", distance);
        return 0;
    }
    else{
        //slog("plane.z: %f",plane.z);
        //slog("distance: %f", distance);
        return 1;
    }
}

Uint8 collision_box_to_plane_z_down(Box b, Plane3D plane){
    float distance;
    if(b.z > 0){
        distance = abs(plane.z) + (abs(b.z) + b.h);
        //slog("distance: %f", distance);
    }
    else{
        distance = plane.z + abs(b.z);
        //slog("distance: %f", distance);
    }
    if(abs(distance) <= 1.0){
        //slog("hit collision in z_down! distance: %f", distance);
        return 0;
    }
    return 1;
}