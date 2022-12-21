#include "projectile.h"
#include "gf3d_draw.h"

Entity* P_new(Entity* source, Entity* target, Uint8 flags, float damage, float speed, float radius, float size, float range, Uint32 color) {
	Entity* p = entity_new();
	Projectile* info = (Projectile*)gfc_allocate_array(sizeof(Projectile),1);

	info->damage = damage;
	info->radius = radius;
	info->source = source;
	info->targetPos = target->position;
	info->flags = flags;
	info->speed = speed;
	info->range = range;
	info->color = color;
	p->customData = info;

	p->target = target;
	p->scale = vector3d(size, size, size);
	p->think = P_think;
	//p->draw = P_draw;
	p->state = ES_hunt;
	p->fireTimer = SDL_GetTicks();
	p->model = gf3d_model_load_full("models/cannon.obj","models/textures/mu_blue2.png");

	return p;
}
void P_think(Entity* self) {
	Projectile* info = (Projectile*)self->customData;
	if (self->state == ES_hunt) {
		/*if (!vector3d_distance_between_less_than(self->position, info->source->position, info->range)) {
			entity_free(self);
			return;
		}*/
		if (info->flags & PF_HOMING) {
			info->targetPos = self->target->position;
		}
		if (vector3d_distance_between_less_than(self->position, info->targetPos, self->scale.x)) {
			self->state = ES_attack;
		}
		else {
			vector3d_move_toward(self->position, info->targetPos, info->speed);
		}
	}
	else if (self->state == ES_attack) {
		if (info->flags & PF_SPLASH) {
			if (info->flags & PF_GROW_BLAST) {
				float r = (SDL_GetTicks() - self->fireTimer) / 512.0f;
				self->scale = vector3d(r, r, r);
				if (r > info->radius) {
					entity_free(self);
				}
			}
		}
		else {
			self->target->damage(self->target, info->damage, self);
		}
	}
}
void P_draw(Entity* self) {
	Projectile* info = (Projectile*)self->customData;
	gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, self->scale.x), self->position, vector3d(1, 1, 1), vector3d(1, 1, 1), gfc_color_hex(info->color), gfc_color(1, 1, 1, 1));
}
void P_explode(Entity* self) {

}