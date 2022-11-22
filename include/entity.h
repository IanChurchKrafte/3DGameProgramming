#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_color.h"
#include "gfc_primitives.h"

#include "gf3d_model.h"

typedef enum
{
    ES_idle = 0,
    ES_hunt = 1,
    ES_dead = 2,
    ES_attack = 3,

    /*for defense*/
    ES_BASE = 4,
    ES_T1 = 5,
    ES_T2 = 6,
    ES_T3 = 7,
}EntityState;

typedef enum
{
    R_bullet = 0,
    R_fire = 1,
    R_melee = 2,
    R_magic = 3,
    R_ice = 4
}EntityResistance;

typedef enum
{
    ET_monster = 0,
    ET_wall = 1,
    ET_turret = 2,
    ET_goo = 3,
    ET_player = 4
}EntityType;


typedef struct Entity_S
{
    Uint8       _inuse;     /**<keeps track of memory usage*/
    Matrix4     modelMat;   /**<orientation matrix for the model*/
    Color       color;      /**<default color for the model*/
    Model      *model;      /**<pointer to the entity model to draw  (optional)*/
    Uint8       hidden;     /**<if true, not drawn*/
    Uint8       selected;
    Color       selectedColor;      /**<Color for highlighting*/
    
    Box         bounds; // for collisions
    int         team;  //same team dont clip
    int         clips;  // if false, skip collisions

    void       (*think)(struct Entity_S *self); /**<pointer to the think function*/
    void       (*update)(struct Entity_S *self, struct Entity_S *player); /**<pointer to the update function*/
    void       (*draw)(struct Entity_S *self); /**<pointer to an optional extra draw funciton*/
    void       (*damage)(struct Entity_S *inflictor, struct Entity_S *self, int damage, int heal); /**<pointer to the damage function*/
    //void       (*damage)(struct Entity_S *self, float damage, struct Entity_S *inflictor); /**<pointer to the think function*/
    void       (*onDeath)(struct Entity_S *self); /**<pointer to an funciton to call when the entity dies*/
    
    EntityState state;
    
    Vector3D    position;  
    Vector3D    velocity;
    Vector3D    acceleration;
        
    Vector3D    scale;
    Vector3D    rotation;
    
    int         health;     /**<entity dies when it reaches zero*/
    // WHATEVER ELSE WE MIGHT NEED FOR ENTITIES
    int                 isPlayer;
    Uint32              points;
    Uint32              currentTime;
    int                 isDead;
    EntityResistance    resistance;
    EntityResistance    attackType;
    int                 attackDamage;
    Vector3D            playerPosition; //for the enemies to be able to face the player
    //Entity              *defenseList;
    Box                 defenseBounds[200]; //list of bounding boxes for the defenses, max is 200
    int                 defenseCount;
    int                 behindWall; //1 if behind wall (start to damage wall), 0 if not
    int                 stateSwitched;
    EntityType          type; //0 for monster, 1 for wall, 2 for turret, 3 for goo
    int                 entityNum;
    /*For entity Num
    monsters 1->10 (in order of fileName)
    wall 1->2 (in order of fileName)
    turret 1->3 (in order of fileName)
    goo 1->5 (slow, fast, dmg, heal, incDmg)
    
    */

    //for jumping
    float       startPosition;
    Uint32      isJumping;
    Uint32      isDescending;
    
    struct Entity_S *target;    /**<entity to target for weapons / ai*/
    
    void *customData;   /**<IF an entity needs to keep track of extra data, we can do it here*/
}Entity;

/**
 * @brief initializes the entity subsystem
 * @param maxEntities the limit on number of entities that can exist at the same time
 */
void entity_system_init(Uint32 maxEntities);

/**
 * @brief provide a pointer to a new empty entity
 * @return NULL on error or a valid entity pointer otherwise
 */
Entity *entity_new();

/**
 * @brief free a previously created entity from memory
 * @param self the entity in question
 */
void entity_free(Entity *self);


/**
 * @brief Draw an entity in the current frame
 * @param self the entity in question
 */
void entity_draw(Entity *self);

/**
 * @brief draw ALL active entities
 */
void entity_draw_all();

/**
 * @brief Call an entity's think function if it exists
 * @param self the entity in question
 */
void entity_think(Entity *self);

/**
 * @brief run the think functions for ALL active entities
 */
void entity_think_all();

/**
 * @brief run the update functions for ALL active entities
 * @param playerPos the position of the player
 */
void entity_update_all(Entity *player);

/**
 * @brief run damage function in this entity
 * @param inflictor the entity doing the damage
 * @param self the entity receiving the damage
 * @param damage int, damage points
 * @param heal 1 if healing 0 if not
*/
void entity_damage(Entity *inflictor, Entity *self, int damage, int heal);

/**
 * @brief run the onDeath function for this entity, freeing it from the system
 * @param self the entity in function
*/
void entity_onDeath(Entity *self);

#endif
