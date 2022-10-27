#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "entity.h"

#define PF_IGNORE_BUILDINGS		0x01
#define PF_IGNORE_ENEMIES		0x02
#define PF_HEAL_BY_DAMAGE		0x04
#define	PF_SAP_HEALTH			0x08
#define PF_BOUNCE				0x10
#define PF_HOMING				0x20

typedef struct Projectile_S {
	Element el;
	float damage;
	Vector3D origin;
	float range;
	Entity* source;
	Uint8 bounces;
	Uint8 flags;
}Projectile;

void P_think(Entity* self);

#endif // !__PROJECTILE_H__