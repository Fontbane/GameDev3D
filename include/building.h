#ifndef __BUILDING_H__
#define __BUILDING_H__
#include "entity.h"
#include "gfc_vector.h"
#include "gfc_hashmap.h"

#define BF_DEFENSE	0x01
#define BF_SPLASH	0x02
#define BF_RESOURCE	0x04
#define BF_AIR		0x08
#define BF_GROUND	0x10
#define BF_FIRE		0x20
#define BF_WALL		0x40
#define BF_HALL		0x80

#define NORMAL_CANNON	0
#define GIANT_CANNON	1
#define DOUBLE_CANNON	2

#define NORMAL_ROCKET	0
#define MULTI_ROCKET	1
#define HOMING_ROCKET	2

#define NORMAL_MORTAR	0
#define MULTI_MORTAR	1
#define MORTAR_BLOWUP	2

#define NORMAL_WIZARD_TOWER	0
#define IVORY_TOWER			1
#define DARK_TOWER			2

#define NORMAL_INFERNO		0
#define MELTDOWN_INFERNO	1
#define HEATSOAK_INFERNO	2

#define	NORMAL_WALL			0
#define	ELECTRO_WALL		1
#define	SLUDGE_WALL			2

typedef enum Building {
	BUILDING_NONE,
	WALL,
	HALL,
	HUT,
	LAB,
	MINE,
	LEY,
	CANNON,
	ROCKETS,
	MORTAR,
	WIZARD_TOWER,
	INFERNO_TOWER,
	BUILDINGS_MAX
}Building;

typedef struct BuildingInfo {
	Building id;
	char* name;
	float range;
	float vulnerability;
	Uint16 damage;//per hit
	Uint16 tickstofire;
	Uint16 maxHealth;
	Uint16 cost;
	Uint8 targets;
	Uint8 flags;
	Uint8 size;
	Uint8 mode;
}BuildingInfo;

typedef struct BuildingData {
	Building id;
	BuildingInfo* info;
	SDL_Point villagepos;
	Uint16 health;
	Uint8 level;
	Uint8 mode;
}BuildingData;


HashMap* B_statmap;

void B_think(Entity* self);

int B_fire(Entity* building, Entity* target);

void B_break(Entity* building, Entity* causeOfDeath);

void B_apply_upgrade(Entity* building, Uint8 mode);

void B_level_up(Entity* building);

void B_inferno_think(Entity* building);

void B_inferno_fire(Entity* building);

void B_rocket_fire(Vector3D position, Uint16 damage);

void B_rocket_think(Entity* self);

void B_rocket_draw(Entity* self);

void B_mortar_explode(Entity* self);

void B_inferno_meltdown(Entity* self);

void B_init_statmap();

void B_init_buildinginfo(SJson* json, char* buildingName, BuildingInfo* biptr);

Entity* B_spawn(Vector3D position, char* name);

extern Entity* B_find_nearest(Entity* building);

#endif /*__BUILDING_H__*/