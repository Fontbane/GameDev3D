#include "building.h"
#include "save.h"

void building_entify(Entity* self, BuildingInfo info) {
	BuildingData* data = self->customData;
	self->health = data->health;
	self->position = vector3d(data->villagepos.x,data->villagepos.y,0);
}

void B_think(Entity* self) {
	BuildingData* data = self->customData;
	if (SDL_GetTicks()-self->fireTimer < data->info->tickstofire) {
		Entity* nearest = B_find_nearest(self);
		if (nearest) {
			B_fire(self, nearest);
		}
	}
}