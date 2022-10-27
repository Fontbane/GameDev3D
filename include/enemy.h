#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "entity.h"

#define EF_MELEE			0x01
#define EF_AIR				0x02
#define EF_GROUND			0x04
#define EF_FIRE				0x08
#define EF_ICE				0x10
#define EF_SPLASH			0x20
#define EF_SPAWN_MINIS		0x40
#define EF_TARGET_DEFENSE	0x80

typedef enum Enemy {
	NONE,
	ARCHER,
	BUSTER,
	GOLEM,
	GOLEMITE,
	SNOW_GOLEM,
	BAT,
	DRAGON,
	SHAMBLER,
	MAX
} Enemy;

typedef struct EnemyInfo {
	Enemy id;
	Uint8 weight;
	float baseSpeed;//units moved per tick
	float damage;//per hit
	Uint16 fireTicks;
	float range;
	Uint16 maxHealth;
	Uint16 drops;
	Uint8 flags;
} EnemyInfo;

int M_fire(Entity* attacker, Entity* target);

int M_melee(Entity* attacker, Entity* target);

void M_die(Entity* enemy, Entity* causeOfDeath);

void M_think(Entity* self);

extern Entity* M_find_nearest(Entity* enemy);

#endif /*__ENEMY_H__*/