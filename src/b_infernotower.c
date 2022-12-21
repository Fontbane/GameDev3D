#include "building.h"
#include "gf3d_draw.h"
#include "spells.h"
#include "enemy.h"

void B_inferno_think(Entity* self) {
	if (!self->target || self->target->state == ES_dead) {
		self->fireTimer = 0;
		self->target = B_find_nearest(self);
	}
	if (self->target&& (SDL_GetTicks() - self->fireTimer) % 128 == 0) {
		B_inferno_fire(self);
	}
}

void B_inferno_fire(Entity* building) {
	BuildingInfo* info = (BuildingInfo*)building->customData;
	float firefactor = min(1, 1 + (SDL_GetTicks() - building->fireTimer) / 128.0f);
	if (info->mode == HEATSOAK_INFERNO) {
		firefactor += building->health / info->maxHealth;
	}
	building->target->damage(building->target, info->damage*firefactor, 1);
}

void B_inferno_draw(Entity* self) {
	float firefactor = min(1, 1 + (SDL_GetTicks() - self->fireTimer) / 128.0f);
	gf3d_draw_edge_3d(
		gfc_edge3d_from_vectors(self->position, self->target->position),
		vector3d(0, 0, 0),
		vector3d(0, 0, 0),
		vector3d(1, 1, 1),
		0.5f,
		gfc_color(1, 1 / firefactor, 0, 1)
	);
}

void B_inferno_meltdown(Entity* self) {
	S_spawn(self->position, "pyro_spell");
}