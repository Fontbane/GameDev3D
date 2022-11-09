#include "enemy.h"

void M_golem_die(Entity* self) {
	M_spawn(self->position, "golemite");
	M_spawn(self->position, "golemite");
	entity_free(self);
}