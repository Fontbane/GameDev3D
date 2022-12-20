#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "entity.h"
#include "gfc_hashmap.h"
#include "gfc_primitives.h"

#define EF_MELEE			0x01
#define EF_AIR				0x02
#define EF_GROUND			0x04
#define EF_FIRE				0x08
#define EF_ICE				0x10
#define EF_SPLASH			0x20
#define EF_ENEMY			0x40
#define EF_TARGET_DEFENSE	0x80

typedef enum Enemy {
	ENEMY_NONE,
	ARCHER,
	BUSTER,
	GOLEM,
	GOLEMITE,
	SNOW_GOLEM,
	BAT,
	DRAGON,
	SHAMBLER,
	ENEMIES_MAX
} Enemy;

typedef struct EnemyInfo {
	Enemy id;
	int weight;
	float baseSpeed;//units moved per tick
	float damage;//per hit
	int tickstofire;
	float range;
	int maxHealth;
	int drops;
	char* name;
	float size;
	Uint8 flags;
} EnemyInfo;

HashMap* M_statmap;

int M_fire(Entity* attacker, Entity* target);

int M_melee(Entity* attacker, Entity* target);

void M_die(Entity* enemy);

void M_think(Entity* self);

void M_update(Entity* self);

Entity* M_spawn(Vector3D position, char* name);

void M_damage(Entity* self, float damage, Entity* inflictor);

void M_golem_die(Entity* self);

void M_shambler_fire(Entity* attacker, Entity* target);

void M_statmap_init();

void M_init_enemyinfo(SJson* json, char* name, EnemyInfo* mptr);

extern Entity* M_find_nearest(Entity* enemy);

#endif /*__ENEMY_H__*/