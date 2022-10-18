#include "building.h"
#include "save.h"

void building_entify(Entity* self, BuildingInfo info) {
	BuildingData* data = self->customData;
	self->health = data->health;

}