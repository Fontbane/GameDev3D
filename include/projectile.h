#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "entity.h"

#define PF_IGNORE_BUILDINGS		0x01
#define PF_IGNORE_ENEMIES		0x02
#define PF_HEAL_BY_DAMAGE		0x04
#define	PF_ICE					0x08
#define PF_BOUNCE				0x10
#define PF_HOMING				0x20
#define PF_SPLASH				0x40
#define PF_FIRE					0x80

typedef struct Projectile_S {
	Element el;
	float damage;
	Vector3D origin;
	Entity* source;
	Entity* target;
	Vector3D targetPos;
	Uint8 bounces;
	Uint8 flags;
}Projectile;

Entity* P_new(Entity* source, Vector3D targetPos, Entity* target, Uint8 flags, float damage);
void P_think(Entity* self);
void P_explode(Entity* self);

#endif // !__PROJECTILE_H__