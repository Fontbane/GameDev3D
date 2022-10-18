#ifndef __BUILDING_H__
#define __BUILDING_H__
#include "entity.h"
#include "gfc_vector.h"

#define BF_DEFENSE	0x01
#define BF_SPLASH	0x02
#define BF_RESOURCE	0x04
#define BF_AIR		0x08
#define BF_GROUND	0x10
#define BF_FIRE		0x20
#define BF_WALL		0x40
#define BF_HALL		0x80

typedef enum Building {
	NONE,
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
	MAX
}Building;

typedef struct BuildingData {
	Building id;
	SDL_Point villagepos;
	Uint16 health;
	Uint8 level;
	Uint8 mode;
}BuildingData;

typedef struct BuildingInfo {
	Building id;
	float range;
	float vulnerability;
	float damage;//per hit
	float timetohit;
	Uint16 maxHealth;
	Uint16 cost;
	Uint8 targets;
	Uint8 flags;
	Uint8 size;
}BuildingInfo;

void building_entify(Entity* self, BuildingInfo info);

int B_fire(Entity* building, Entity* projectile, Entity* target);

int B_melee(Entity* building, Entity* target);

void B_die(Entity* building, Entity* causeOfDeath);

void B_find_nearest(Entity* building);

#endif /*__BUILDING_H__*/