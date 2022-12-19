#include "rayCast.h"
#include "hitBoxEnt.h"
#include "agumon.h"

Entity * rayCast(Entity *self, float distance, Entity *player){
    Entity *entityList[512];
    for(int i=0; i<512; i++)
        entityList[i] = player->entityList[i];

    //self is the shooting entity
    //seems to work when trying to raycast into the -y side, very inconsistent when ray casting into the +y side
    Vector3D playerPos = self->position;
    playerPos.z += 5;
    //float distance = 5.0; //10 actually makes a distance of 25,  5 = 12.5 distance and so on
    float pitch = self->rotation.x;
    float yaw = self->rotation.z;
    float sp = sinf(pitch);
    float cp = cosf(pitch) - M_PI_2;
    float sy = sinf(yaw - M_PI_2);
    float cy = cosf(yaw - M_PI_2);

    Vector3D dir = vector3d(cp*cy, cp*sy, sp);
    float x = playerPos.x + dir.x * distance;
    float y = playerPos.y + dir.y * distance;
    float z = playerPos.z + dir.z * distance;
    Vector3D testPoint = vector3d(x, y, z);
    //Entity *agu = agumon_new(testPoint);
    //slog("testPoint: %f, %f, %f", x, y, z);
    Edge3D ray; //more like a line segment then a ray
    ray.a = playerPos;
    ray.b = testPoint;
    Vector3D *poc = &playerPos, *normal = NULL;
    //slog("edge length: %f", edge3DLength(ray));
    // self->position = testPoint;
    slog("poc1: %f, %f, %f", poc->x, poc->y, poc->z);
    for(int j=0; j<512; j++){
        if(!entityList[j])
            continue;

        if(gfc_edge_box_test(ray, entityList[j]->bounds, poc, normal) && entityList[j]->type == ET_monster){
            slog("intersected with entity type: %i", entityList[j]->type);
            slog("intersected entity pos: %f, %f, %f", entityList[j]->position.x, entityList[j]->position.y, entityList[j]->position.z);
            slog("poc2: %f, %f, %f", poc->x, poc->y, poc->z);
            return entityList[j];
            //entity_damage(self, entityList[j], self->attackDamage, 0);
            //return entityList[j];
        }
        else if(gfc_edge_box_test(ray, entityList[j]->bounds, poc, normal) && entityList[j]->type != ET_monster){
            slog("interected with non-monster ent");
        }
        else
            slog("missed");
    }
    return NULL;
}

float distance3D(Vector3D start, Vector3D end){
    float dis1 = powf((end.x - start.x), 2);
    float dis2 = powf((end.y - start.y), 2);
    float dis3 = powf((end.z - start.z), 2);

    return sqrtf(dis1 + dis2 + dis3);
}

float edge3DLength(Edge3D e){
    float a,b,c;
    a = e.a.x - e.b.x;
    b = e.a.y - e.b.y;
    c = e.a.z - e.b.z;
    return vector3d_magnitude(vector3d(a,b,c));
}