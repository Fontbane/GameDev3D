#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"

#include "entity.h"
#include "building.h"
#include "enemy.h"

typedef struct
{
    Entity *entity_list;
    Entity* building_list;
    Uint32  entity_count;
    
}EntityManager;

static EntityManager entity_manager = {0};

void entity_system_close()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        entity_free(&entity_manager.entity_list[i]);
        entity_free(&entity_manager.building_list[i]);
    }
    free(entity_manager.entity_list);
    free(entity_manager.building_list);
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity_system closed");
}

void entity_system_init(Uint32 maxEntities)
{
    entity_manager.entity_list = gfc_allocate_array(sizeof(Entity),maxEntities);
    if (entity_manager.entity_list == NULL)
    {
        slog("failed to allocate entity list, cannot allocate ZERO entities");
        return;
    }
    entity_manager.entity_count = maxEntities;
    atexit(entity_system_close);
    slog("entity_system initialized");
}

Entity *entity_new()
{
    int i;

    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet, so we can!
        {
            entity_manager.entity_list[i]._inuse = 1;
            gfc_matrix_identity(entity_manager.entity_list[i].modelMat);
            entity_manager.entity_list[i].scale.x = 1;
            entity_manager.entity_list[i].scale.y = 1;
            entity_manager.entity_list[i].scale.z = 1;
            
            entity_manager.entity_list[i].color = gfc_color(1,1,1,1);
            entity_manager.entity_list[i].selectedColor = gfc_color(1,1,1,1);
            
            entity_manager.entity_list[i].ticksSinceStatus = 0;
            return &entity_manager.entity_list[i];
        }
    }
    slog("entity_new: no free space in the entity list");
    return NULL;
}

Entity* building_new()
{
    int i;

    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.building_list[i]._inuse)// not used yet, so we can!
        {
            entity_manager.building_list[i]._inuse = 1;
            gfc_matrix_identity(entity_manager.building_list[i].modelMat);
            entity_manager.building_list[i].scale.x = 1;
            entity_manager.building_list[i].scale.y = 1;
            entity_manager.building_list[i].scale.z = 1;

            entity_manager.building_list[i].color = gfc_color(1, 1, 1, 1);
            entity_manager.building_list[i].selectedColor = gfc_color(1, 1, 1, 1);

            entity_manager.building_list[i].ticksSinceStatus = 0;
            return &entity_manager.building_list[i];
        }
    }
    slog("building_new: no free space in the building list");
    return NULL;
}

void entity_free(Entity *self)
{
    if (!self)return;
    //MUST DESTROY
    gf3d_model_free(self->model);
    memset(self,0,sizeof(Entity));
}


void entity_draw(Entity *self)
{
    if (!self)return;
    if (self->hidden)return;
    gf3d_model_draw(self->model,self->modelMat,gfc_color_to_vector4f(self->color),vector4d(1,1,1,1));
    if (self->selected)
    {
        gf3d_model_draw_highlight(
            self->model,
            self->modelMat,
            gfc_color_to_vector4f(self->selectedColor));
    }
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_draw(&entity_manager.entity_list[i]);
        if (!entity_manager.building_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_draw(&entity_manager.building_list[i]);
    }
}

void entity_think(Entity *self)
{
    if (!self)return;
    if (self->think)self->think(self);
}

void entity_think_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_think(&entity_manager.entity_list[i]);
        if (!entity_manager.building_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_think(&entity_manager.building_list[i]);
    }
}


void entity_update(Entity *self)
{
    if (!self)return;
    // HANDLE ALL COMMON UPDATE STUFF
    
    vector3d_add(self->position,self->position,self->velocity);
    vector3d_add(self->velocity,self->acceleration,self->velocity);
    
    gfc_matrix_identity(self->modelMat);
    
    gfc_matrix_scale(self->modelMat,self->scale);
    gfc_matrix_rotate_by_vector(self->modelMat,self->modelMat,self->rotation);
    gfc_matrix_translate(self->modelMat,self->position);
    
    if (self->update)self->update(self);
}

void entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_update(&entity_manager.entity_list[i]);
        entity_update(&entity_manager.building_list[i]);
    }
}

Entity* B_find_nearest(Entity* building) {
    BuildingData* data = building->customData;
    BuildingInfo* info = data->info;
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.entity_list[i]._inuse) {
            float d = vector3d_magnitude_between(entity_manager.entity_list[i].position, building->position);
            if (d <= info->range && d >= info->vulnerability) {
                return &entity_manager.entity_list[i];
            }
        }
    }
    return NULL;
}

Vector3D vector3d_move_toward(Vector3D current, Vector3D target, float maxDelta) {
    Vector3D mov;
    vector3d_sub(mov, target, current);
    float mag = vector3d_magnitude(mov);
    if (mag <= maxDelta || mag == 0) {
        return target;
    }
    vector3d_scale(mov, mov, maxDelta / mag);
    vector3d_add(mov, current, mov);
    return mov;
}

Entity* M_find_nearest(Entity* enemy) {
    EnemyInfo* info = enemy->customData;
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.building_list[i]._inuse&& entity_manager.building_list[i].state!=ES_dead) {
            float d = vector3d_magnitude_between(entity_manager.building_list[i].position, enemy->position);
            if (!enemy->target) {
                enemy->target = &entity_manager.building_list[i];
            }
            else if (vector3d_magnitude_between(enemy->target->position, enemy->position) <= d) {
                //if (info->flags&EF_MELEE&&
                //  ((BuildingData*)entity_manager.building_list[i].customData)->info->flags&BF_WALL)
                if (info->flags & EF_TARGET_DEFENSE && 
                    ((BuildingData*)entity_manager.building_list[i].customData)->info->flags & ~BF_DEFENSE)
                    continue;
                enemy->target = &entity_manager.building_list[i];
            }
            if (d <= info->range) {
                return &entity_manager.building_list[i];
            }
        }
    }
    return enemy->target;
}

/*eol@eof*/
