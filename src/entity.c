#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"

#include "entity.h"
#include "building.h"
#include "enemy.h"
#include "spells.h"

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
    entity_manager.building_list = gfc_allocate_array(sizeof(Entity), maxEntities);
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
            entity_manager.building_list[i].fireTimer = SDL_GetTicks();
            
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

            entity_manager.building_list[i].fireTimer = SDL_GetTicks();
            entity_manager.building_list[i].ticksSinceStatus = 0;
            slog("New building at index %i", i);
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
    if (self->draw)self->draw(self);
    if (self->model) {
        gf3d_model_draw(self->model, self->modelMat, gfc_color_to_vector4f(self->color), vector4d(1, 1, 1, 1));
        if (self->selected)
        {
            gf3d_model_draw_highlight(
                self->model,
                self->modelMat,
                gfc_color_to_vector4f(self->selectedColor));
        }
    }
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (entity_manager.entity_list[i]._inuse)// not used yet
        {
            entity_draw(&entity_manager.entity_list[i]);
        }
        if (entity_manager.building_list[i]._inuse)// not used yet
        {
            entity_draw(&entity_manager.building_list[i]);
        }
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
        if (entity_manager.entity_list[i]._inuse)// not used yet
        {
            entity_think(&entity_manager.entity_list[i]);
        }
        if (entity_manager.building_list[i]._inuse)// not used yet
        {
            entity_think(&entity_manager.building_list[i]);
        }
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
        if (entity_manager.entity_list[i]._inuse)// not used yet
        {
            entity_update(&entity_manager.entity_list[i]);
        }
        if (entity_manager.building_list[i]._inuse)// not used yet
        {
            entity_update(&entity_manager.building_list[i]);
        }
    }
}

Entity* B_find_nearest(Entity* building) {
    BuildingInfo* info = (BuildingInfo*) building->customData;
    //slog("%s looking for nearby enemies", info->name);
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.entity_list[i]._inuse&& entity_manager.entity_list[i].state!=ES_notarget/* &&
            (entity_manager.entity_list[i].info&ENTTYPE_ENEMY_AIR&&info->flags&BF_AIR||
                entity_manager.entity_list[i].info & ENTTYPE_ENEMY_GRD && info->flags & BF_GROUND)*/) {
            float d = vector3d_magnitude_between(entity_manager.entity_list[i].position, building->position);
            if (/*d <= info->range &&*/ d >= info->vulnerability) {
                slog("%s found an enemy!", info->name);
                return &entity_manager.entity_list[i];
            }
        }
    }
    return NULL;
}

Vector3D vector3d_move_toward(Vector3D current, Vector3D target, float maxDelta) {
    Vector3D mov;
    vector3d_sub(mov, target, current);
    vector3d_normalize(&mov);
    vector3d_scale(mov, mov, maxDelta);
    return mov;
}

Entity* M_find_nearest(Entity* enemy) {
    EnemyInfo* info = (EnemyInfo*)enemy->customData;
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.building_list[i]._inuse && entity_manager.building_list[i].state!=ES_dead) {
            float d = vector3d_magnitude_between(entity_manager.building_list[i].position, enemy->position);
            if (info->range==0||d <= info->range) {
                //if (info->flags&EF_MELEE&&
                //  ((BuildingData*)entity_manager.building_list[i].customData)->info->flags&BF_WALL)
                if (info->flags & EF_TARGET_DEFENSE &&
                    entity_manager.building_list[i].info & ENTTYPE_RESOURCE) {
                    continue;
                }
                enemy->target = &entity_manager.building_list[i];
            }
        }
    }
    return enemy->target;
}

void sphere_damage(Sphere sphere, float baseDamage, Entity* inflictor) {
    Vector3D v3 = vector3d(sphere.x, sphere.y, sphere.z);
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.entity_list[i]._inuse&&vector3d_magnitude_between(entity_manager.entity_list[i].position, v3) <= sphere.r) {
            entity_manager.entity_list[i].damage(&entity_manager.entity_list[i], baseDamage, inflictor);
        }
    }
}

void S_effect(Entity* spell) {
    SpellInfo* info = (SpellInfo*)spell->customData;
    if (info->targetEnemies == 0) {
        for (int i = 0; i < entity_manager.entity_count; i++) {
            if (entity_manager.building_list[i]._inuse && entity_manager.entity_list[i].state != ES_dead) {
                BuildingInfo* info=(BuildingInfo*)entity_manager.entity_list[i].customData;
                entity_manager.entity_list[i].health=min(entity_manager.entity_list[i].health+info->maxHealth/10,info->maxHealth);
            }
        }
        return;
    }
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.entity_list[i]._inuse && entity_manager.entity_list[i].state != ES_notarget) {
            switch (info->id) {
            case FREEZE_SPELL:
                entity_manager.entity_list[i].info |= STATUS_FROZEN;
                break;
            case SLUDGE_SPELL:
                entity_manager.entity_list[i].info |= STATUS_SLUDGE;
                break;
            case SMITE_SPELL:
                entity_free(&entity_manager.entity_list[i]);
                break;
            case PYRO_SPELL:
                entity_manager.entity_list[i].info |= STATUS_BURNED;
                break;
            }
            entity_manager.entity_list[i].ticksSinceStatus = SDL_GetTicks() + info->duration;
        }
    }
}

void B_choose_location(BuildingInfo* info) {
    game.state = Selecting;

    game.selection = gfc_box(0,0,0,info->size/128.0f,1/128.0f,info->size/128.0f);
    game.name=info->id;
}

Uint8 B_check_location(Box box) {
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.building_list[i]._inuse && gfc_box_overlap(entity_manager.building_list[i].bounds, box)) {
            return 0;
        }
    }
    return 1;
}

void B_at_location(Vector3D position) {
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.building_list[i]._inuse && gfc_point_in_box(position, entity_manager.building_list[i].bounds)) {
            BuildingInfo* info = (BuildingInfo*)entity_manager.building_list[i].customData;
            if (info->id == HUT) {
                entity_manager.building_list[i].health = info->maxHealth;
                entity_manager.building_list[i].state = ES_idle;
                return;
            }
            game.state = Selected;
            game.selected = entity_manager.building_list + i;
            slog("Selected %s", info->name);
            return;
        }
    }
    game.state = Day;
}

Vector2D buyMenuBounds[10][2] = {
    {{1,61},{119,179}},//DOUBLE CANNON
    {{121,61},{239,179}},//GIANT CANNON
    {{181,241},{199,359}},//MULTI MORTAR
    {{201,241},{319,359}},//BLOWUP MORTAR
    {{301,61},{419,179}},//MULTI ROCKET
    {{421,61},{539,179}},//HOMING ROCKET
    {{1,421},{119,599}},//IVORY TOWER
    {{121,421},{239,599}},//DARK TOWER
    {{301,421},{419,599}},//HEATSOAK INFERNO
    {{421,421},{539,599}}//MELTDOWN INFERNO
};

void B_try_buy_upgrade(Vector2D position) {
    int i;
    Building id=0;
    int mode=0;
    if (position.y > 61 && position.y < 179) {
        if (position.x > 1 && position.x < 119) {
            id = CANNON;
            mode = DOUBLE_CANNON;
        }
        else if (position.x > 121 && position.x < 239) {
            id = CANNON;
            mode = GIANT_CANNON;
        }
        else if (position.x > 301 && position.x < 419) {
            id = ROCKETS;
            mode = MULTI_ROCKET;
        }
        else if (position.x > 421 && position.x < 539) {
            id = ROCKETS;
            mode = HOMING_ROCKET;
        }
    }
    else if (position.y > 241 && position.y > 359) {
        if (position.x > 181 && position.x < 299) {
            id = MORTAR;
            mode = MULTI_MORTAR;
        }
        else if (position.x > 301 && position.x < 419) {
            id = MORTAR;
            mode = MORTAR_BLOWUP;
        }
    }
    else if (position.y>421 && position.y>599) {
        if (position.x > 1 && position.x < 119) {
            id = WIZARD_TOWER;
            mode = IVORY_TOWER;
        }
        else if (position.x > 121 && position.x < 239) {
            id = WIZARD_TOWER;
            mode = DARK_TOWER;
        }
        else if (position.x > 301 && position.x < 419) {
            id = INFERNO_TOWER;
            mode = HEATSOAK_INFERNO;
        }
        else if (position.x > 421 && position.x < 539) {
            id = INFERNO_TOWER;
            mode = MELTDOWN_INFERNO;
        }
    }
    if (id == 0 || mode == 0) {
        return;
    }
    for (i = 0; i < entity_manager.entity_count; i++) {
        if (entity_manager.building_list[i]._inuse) {
            BuildingInfo* binfo=(BuildingInfo*)entity_manager.building_list[i].customData;
            if (binfo->id == id) {
                B_apply_upgrade(&entity_manager.building_list[i], mode);
            }
        }
    }
}

/*eol@eof*/
