#ifndef __SPELLS_H__
#define __SPELLS_H__
#include "entity.h"
#include "gfc_hashmap.h"

typedef enum {
	FREEZE_SPELL,
	HEAL_SPELL,
	SLUDGE_SPELL,
	SMITE_SPELL,
	PYRO_SPELL
}Spell;

typedef struct {
	Spell id;
	char* name;
	Uint8 targetEnemies;
	float radius;
	int tickstofire;
	int duration;
	int cost;
	Color color;
}SpellInfo;

HashMap* S_statmap;

void S_statmap_init();
void S_init_spellinfo(SJson* json, char* name, SpellInfo* sptr);
void S_try_buy(Vector3D position, char* name);
void S_spawn(Vector3D position, char* name);
void S_think(Entity* self);
void S_draw(Entity* self);

extern void S_effect(Entity* spell);

#endif // !__SPELLS_H__