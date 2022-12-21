#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "entity.h"

#define PF_IGNORE_BUILDINGS		0x01
#define PF_IGNORE_ENEMIES		0x02
#define PF_ICE					0x04
#define	PF_GROW_BLAST			0x08
#define PF_ZAP					0x10
#define PF_HOMING				0x20
#define PF_SPLASH				0x40
#define PF_FIRE					0x80

typedef struct Projectile_S {
	float damage;
	float radius; //Maximum radius of a splash projectile
	Entity* source;
	Vector3D targetPos;
	float speed;
	float size; //base size of object to draw
	float range;
	Uint32 color;
	Uint8 flags;
}Projectile;

Entity* P_new(Entity* source, Entity* target, Uint8 flags, float damage, float speed, float radius, float size, float range, Uint32 color);
void P_think(Entity* self);
void P_draw(Entity* self);
void P_explode(Entity* self);

#endif // !__PROJECTILE_H__