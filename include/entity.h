#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_color.h"
#include "gfc_primitives.h"

#include "gf3d_model.h"

#include "gamelocal.h"

#define STATUS_BURNED       0x01
#define STATUS_FROZEN       0x02
#define STATUS_SLUDGE       0x04
#define STATUS_BROKEN       0x08
#define ENTTYPE_BUILDING    0x10
#define ENTTYPE_ENEMY       0x20
#define ENTTYPE_POINT       0x40
#define ENTTYPE_LINE        0x80

typedef enum
{
    ES_idle = 0,
    ES_hunt,
    ES_dead,
    ES_attack,
    ES_notarget
}EntityState;

typedef enum Element {
    NONE,
    FIRE,
    ICE,
    ELECTRIC,
    MAX
}Element;

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

    Vector3D    minCorner;
    Vector3D    maxCorner;
    void       (*think)(struct Entity_S *self); /**<pointer to the think function*/
    void       (*update)(struct Entity_S *self); /**<pointer to the update function*/
    void       (*draw)(struct Entity_S *self); /**<pointer to an optional extra draw funciton*/
    void       (*damage)(struct Entity_S *self, float damage, struct Entity_S *inflictor); /**<pointer to the think function*/
    void       (*onDeath)(struct Entity_S *self); /**<pointer to an funciton to call when the entity dies*/
    
    EntityState state;
    
    Vector3D    position;  
    Vector3D    velocity;
    Vector3D    acceleration;
        
    Vector3D    scale;
    Vector3D    rotation;

    Uint8       info;

    Uint32      ticksSinceStatus;
    Uint32      fireTimer;
    
    Uint32      health;     /**<entity dies when it reaches zero*/
    // WHATEVER ELSE WE MIGHT NEED FOR ENTITIES
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
 */
void entity_update_all();

Entity* building_new();

Vector3D vector3d_move_toward(Vector3D current, Vector3D target, float rate);

void sphere_damage(Sphere sphere, Uint16 baseDamage);

Uint8 B_check_location(Box box);

Uint8 B_at_location(Vector3D position);

#endif
