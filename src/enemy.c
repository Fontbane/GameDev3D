#include "simple_logger.h"
#include "enemy.h"

void M_think(Entity* self) {
	EnemyInfo* info = self->customData;
	if (self->state == ES_attack) {
		if (self->target->health > 0 && SDL_GetTicks() - self->fireTimer <= info->fireTicks) {
			M_fire(self, self->target);
		}
		else {
			self->state = ES_hunt;
			self->target = NULL;
		}
	}
	if (!self->target) {
		M_find_nearest(self);
	}
	if (!self->target) {
		slog("No target found");
		M_die(self, NULL);
	}
	if (vector3d_distance_between_less_than(self->position, self->target->position, info->range)) {
		if (info->flags & EF_MELEE) {
			M_melee(self, self->target);
		}
		M_fire(self, self->target);
	}
	else {
		self->position = vector3d_move_toward(self->position, self->target->position, info->baseSpeed);
	}
}