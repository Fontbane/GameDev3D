#include "enemy.h"

void M_golem_die(Entity* self, Entity* causeOfDeath) {
	M_spawn(self->position, "golemite");
	M_spawn(self->position, "golemite");
	entity_free(self);
}