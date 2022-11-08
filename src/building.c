#include <simple_json.h>
#include <simple_logger.h>

#include "building.h"
#include "save.h"

void B_think(Entity* self) {
	BuildingInfo* info = self->customData;
	if (self->state == ES_dead) {
		return;
	}
	if (self->health == 0) {
		B_break(self, NULL);
	}
	if ((SDL_GetTicks()-self->fireTimer)%256==0&&(info->id == MINE)) {
		saveData->gold += info->damage;
		return;
	}
	if ((SDL_GetTicks() - self->fireTimer) % 256 == 0 && (info->id == LEY)) {
		saveData->elixir += info->damage;
		return;
	}
	if (info->id == INFERNO_TOWER) {
		B_inferno_think(self);
		return;
	}
	if (SDL_GetTicks()-self->fireTimer < info->tickstofire||
		(info->id==MORTAR&&info->mode==MULTI_MORTAR||info->id==ROCKETS&&info->mode==MULTI_ROCKET)&& 
		SDL_GetTicks() - self->fireTimer == 10 || SDL_GetTicks() - self->fireTimer == 20) {
		Entity* nearest = B_find_nearest(self);
		if (nearest) {
			B_fire(self, nearest);
			if (info->id == CANNON && info->mode == DOUBLE_CANNON) {
				B_fire(self, nearest);
			}
		}
	}
}

int B_fire(Entity* building, Entity* target) {
	BuildingInfo* info = (BuildingInfo*)building->customData;
	if (info->id == ROCKETS) {
		B_rocket_fire(target->position,info->damage);
	}
	
	return 0;
}

void B_init_statmap() {
	B_statmap = gfc_hashmap_new();
	BuildingInfo cannon = { 0 };
	BuildingInfo doublecannon = { 0 };
	BuildingInfo giantcannon = { 0 };

	BuildingInfo mortar = { 0 };
	BuildingInfo multimortar = { 0 };
	BuildingInfo blowupmortar = { 0 };

	BuildingInfo wiztower = { 0 };
	BuildingInfo ivorytower = { 0 };
	BuildingInfo darktower = { 0 };

	BuildingInfo rocket = { 0 };
	BuildingInfo multirocket = { 0 };
	BuildingInfo homingrocket = { 0 };

	BuildingInfo inferno = { 0 };
	BuildingInfo meltdowninferno = { 0 };
	BuildingInfo heatsoakinferno = { 0 };

	BuildingInfo mine = { 0 };
	BuildingInfo ley = { 0 };
	BuildingInfo lab = { 0 };
	BuildingInfo hut = { 0 };
	BuildingInfo hall = { 0 };

	SJson* json = sj_load("config/buildings.json");
	B_init_buildinginfo(json, "cannon", &cannon);
	B_init_buildinginfo(json, "doublecannon", &doublecannon);
	B_init_buildinginfo(json, "giantcannon", &giantcannon);

	B_init_buildinginfo(json, "mortar", &mortar);
	B_init_buildinginfo(json, "multimortar", &multimortar);
	B_init_buildinginfo(json, "blowupmortar", &blowupmortar);

	B_init_buildinginfo(json, "wiztower", &wiztower);
	B_init_buildinginfo(json, "ivorytower", &ivorytower);
	B_init_buildinginfo(json, "darktower", &darktower);

	B_init_buildinginfo(json, "rocket", &rocket);
	B_init_buildinginfo(json, "multirocket", &multirocket);
	B_init_buildinginfo(json, "homingrocket", &homingrocket);

	B_init_buildinginfo(json, "inferno", &inferno);
	B_init_buildinginfo(json, "meltdowninferno", &meltdowninferno);
	B_init_buildinginfo(json, "heatsoakinferno", &heatsoakinferno);

	B_init_buildinginfo(json, "gold_mine", &mine);
	B_init_buildinginfo(json, "ley_line", &ley);
	B_init_buildinginfo(json, "lab", &lab);
	B_init_buildinginfo(json, "hut", &hut);
	B_init_buildinginfo(json, "hall", &hall);
}

void B_init_buildinginfo(SJson* json, char* buildingName, BuildingInfo* biptr) {
	SJson* building = sj_object_get_value(json, buildingName);
	biptr->name = buildingName;
	sj_get_integer_value(sj_object_get_value(building, "id"), &biptr->id);
	sj_get_integer_value(sj_object_get_value(building, "mode"), &biptr->mode);
	sj_get_integer_value(sj_object_get_value(building, "cost"), &biptr->cost);
	sj_get_integer_value(sj_object_get_value(building, "tickstofire"), &biptr->tickstofire);
	sj_get_integer_value(sj_object_get_value(building, "maxHealth"), &biptr->maxHealth);
	sj_get_integer_value(sj_object_get_value(building, "damage"), &biptr->damage);
	sj_get_float_value(sj_object_get_value(building, "range"), &biptr->range);
	sj_get_float_value(sj_object_get_value(building, "vulnerability"), &biptr->vulnerability);
	gfc_hashmap_insert(B_statmap, buildingName, biptr);
}


void B_apply_upgrade(Entity* building, Uint8 mode) {
	if (mode == 0) return;
	BuildingInfo* info = (BuildingInfo*)building->customData;
	char* name = info->name;
	switch (info->id) {
	case CANNON:
		name = mode == 1 ? "doublecannon" : "giantcannon";
		break;
	case MORTAR:
		name = mode == 1 ? "multimortar" : "blowupmortar";
		break;
	case ROCKETS:
		name = mode == 1 ? "multirockets" : "homingrockets";
		break;
	case WIZARD_TOWER:
		name = mode == 1 ? "ivorytower" : "darktower";
		break;
	case INFERNO_TOWER:
		name = mode == 1 ? "meltdowninferno" : "heatsoakinferno";
		break;
	}
	BuildingInfo* newinfo = (BuildingInfo*)gfc_hashmap_get(B_statmap, name);
	if (newinfo->cost > saveData->gold) {
		slog("Come back when you're a little, mmm, richer, okay?");
		return;
	}
	else {
		saveData->gold -= newinfo->cost;
		building->customData = newinfo;
		building->health = newinfo->maxHealth;
		building->model = gf3d_model_load(name);
		slog("Upgraded to %s", name);
	}

}

Entity* B_try_buy(char* name) {
	BuildingInfo* info = (BuildingInfo*)gfc_hashmap_get(B_statmap, name);
	if (!info) {
		slog("Data for building named %s not found", name);
		return NULL;
	}
	else if (info->cost > saveData->gold) {
		slog("Come back when you're a little, mmm, richer, okay?");
		return;
	}
	else{
		saveData->gold -= info->cost;
		if (info->id == LEY || info->id == MINE) {
			info->cost *= 8;
		}
		return B_spawn(vector3d(0, 0, 0), info);
	}
}

Entity* B_spawn(Vector3D position, BuildingInfo* info) {
	Entity* b = building_new();
	if (!b) {
		slog("Unable to spawn new building");
		return NULL;
	}

	b->position = position;
	b->health = info->maxHealth;

	b->customData = info;

	b->think = B_think;

	Box bounds = {
		.x = position.x - info->size / 2.0f,
		.y = position.y - info->size / 2.0f,
		.z = 0,

		.w = info->size,
		.h = info->size,
		.d = info->size
	};
	b->bounds = bounds;
	b->model = gf3d_model_load(info->name);
	saveData->totalBuildings[info->id]++;
}

void B_break(Entity* building, Entity* causeOfDeath) {
	building->info &= ES_dead;
	building->color.a = 0.5f;
	BuildingInfo* info=(BuildingInfo*)building->customData;

}