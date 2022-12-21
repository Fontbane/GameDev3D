#include <simple_json.h>
#include <simple_logger.h>

#include "building.h"
#include "save.h"
#include "gfc_primitives.h"
#include "gf3d_draw.h"
#include "gf2d_mouse.h"
#include "projectile.h"

void B_think(Entity* self) {
	if (self->state == ES_dead) {
		return;
	}
	BuildingInfo* info = (BuildingInfo*)self->customData;
	if (self->health == 0) {
		B_break(self);
		return;
	}
	if ((SDL_GetTicks()-self->fireTimer)>=info->tickstofire&&(info->id == MINE)) {
		saveData->gold += info->damage;
		self->fireTimer = SDL_GetTicks();
		return;
	}
	if ((SDL_GetTicks() - self->fireTimer) >= info->tickstofire && (info->id == LEY)) {
		saveData->elixir += info->damage;
		self->fireTimer = SDL_GetTicks();
		return;
	}
	if (game.state != Night) {
		return;
	}
	if (info->id == INFERNO_TOWER) {
		B_inferno_think(self);
		return;
	}
	else if (info->id == MORTAR && info->mode == MULTI_MORTAR || info->id == ROCKETS && info->mode == MULTI_ROCKET) {
		B_multi_think(self);
		return;
	}
	if (SDL_GetTicks()-self->fireTimer >= info->tickstofire) {
		if (!self->target||self->target->state==ES_dead)
			self->target = B_find_nearest(self);
		if (self->target) {
			B_fire(self, self->target);
			if (info->id == CANNON && info->mode == DOUBLE_CANNON) {
				B_fire(self, self->target);
			}
		}
		else {
			slog("No target found");
		}
	}
}
void B_damage(Entity* self, float damage, Entity* inflictor) {
	if (damage > self->health) {
		self->health = 0;
		B_break(self);
	}
	else {
		self->health -= damage;
	}
}
void B_multi_think(Entity* self) {
	BuildingInfo* info = (BuildingInfo*)self->customData;
	if (self->state==ES_idle&&SDL_GetTicks() - self->fireTimer >= info->tickstofire){
		B_fire(self, self->target);
		self->state = ES_hunt;
	}
	else if (self->state == ES_hunt && SDL_GetTicks() - self->fireTimer >= 10) {
		B_fire(self, self->target);
		self->state = ES_attack;
	}
	else if (self->state == ES_attack && SDL_GetTicks() - self->fireTimer >= 10) {
		B_fire(self, self->target);
		self->state = ES_idle;
	}
}

int B_fire(Entity* building, Entity* target) {
	BuildingInfo* info = (BuildingInfo*)building->customData;
	if (info->id == ROCKETS) {
		B_rocket_fire(target->position,info->damage);
	}
	float radius = 1;
	float speed = 0.2f;
	Uint8 flags = PF_IGNORE_BUILDINGS;
	Color color = gfc_color(0, 0, 0, 1);
	if (info->id == ROCKETS || info->id == MORTAR) {
		flags |= PF_GROW_BLAST;
		flags |= PF_SPLASH;
		radius = 3;
		if (info->id == ROCKETS && info->mode == HOMING_ROCKET) {
			flags |= PF_HOMING;
		}
	}
	else if (info->id == WIZARD_TOWER&&info->mode==NORMAL_WIZARD_TOWER) {
		flags |= PF_SPLASH;
		flags |= PF_ZAP;
		color.r = 0.25f;
		color.g = 0.75f;
	}
	else if (info->id == WIZARD_TOWER && info->mode == DARK_TOWER) {
		flags |= PF_SPLASH;
		flags |= PF_FIRE;
		color.r = 1;
		color.g = 0.25f;
	}
	else if (info->id == WIZARD_TOWER && info->mode == IVORY_TOWER) {
		flags |= PF_SPLASH;
		flags |= PF_ICE;
		color.b = 1;
		color.g = 0.25f;
	}
	if (info->id == MORTAR || info->id == CANNON && info->mode == GIANT_CANNON) {
		speed = 0.1f;
	}
	
	Entity* proj = P_new(building, target, flags, info->damage, speed, radius, 0.2f, info->range, gfc_color_to_hex(color));
	if (!proj) {
		slog("Could not summon projectile");
	}
	slog("Fire!!");
	return 0;
}

void B_draw(Entity* self) {
	BuildingInfo* info = (BuildingInfo*)self->customData;
	gf3d_draw_circle(gfc_circle(0, 0, info->range), self->position, self->rotation, vector3d(1, 1, 1), gfc_color(1, 1, 1, 0.8f));
	if (info->id == INFERNO_TOWER&&self->target) {
		B_inferno_draw;
	}
}

void B_init_statmap() {
	B_stats = NULL;
	B_stats = (BuildingInfo*)gfc_allocate_array(sizeof(BuildingInfo), 40);
	/*B_statmap = gfc_hashmap_new();
	BuildingInfo* cannon = malloc(sizeof(BuildingInfo));
	BuildingInfo* doublecannon = malloc(sizeof(BuildingInfo));
	BuildingInfo* giantcannon = malloc(sizeof(BuildingInfo));

	BuildingInfo* mortar = malloc(sizeof(BuildingInfo));
	BuildingInfo* multimortar = malloc(sizeof(BuildingInfo));
	BuildingInfo* blowupmortar = malloc(sizeof(BuildingInfo));

	BuildingInfo* wiztower = malloc(sizeof(BuildingInfo));
	BuildingInfo* ivorytower = malloc(sizeof(BuildingInfo));
	BuildingInfo* darktower = malloc(sizeof(BuildingInfo));

	BuildingInfo* rocket = malloc(sizeof(BuildingInfo));
	BuildingInfo* multirocket = malloc(sizeof(BuildingInfo));
	BuildingInfo* homingrocket = malloc(sizeof(BuildingInfo));

	BuildingInfo* inferno = malloc(sizeof(BuildingInfo));
	BuildingInfo* meltdowninferno = malloc(sizeof(BuildingInfo));
	BuildingInfo* heatsoakinferno = malloc(sizeof(BuildingInfo));

	BuildingInfo* mine = malloc(sizeof(BuildingInfo));
	BuildingInfo* ley = malloc(sizeof(BuildingInfo));
	BuildingInfo* lab = malloc(sizeof(BuildingInfo));
	BuildingInfo* hut = malloc(sizeof(BuildingInfo));
	BuildingInfo* hall = malloc(sizeof(BuildingInfo));*/

	SJson* json = sj_load("config/buildings.json");
	json = sj_object_get_value(json, "buildings");

	int i = 3;

	while (i < BUILDINGS_MAX*3) {
		SJson* building = sj_array_get_nth(json, i);
		if (!building) {
			slog("Can't get that building!");
		}
		char* s = sj_get_string_value(sj_object_get_value(building, "name"));
		if (!s) {
			i++;
			sj_free(building);
			continue;
		}
		B_stats[i].name = (char*)malloc(strlen(s));
		if (!B_stats[i].name) {
			slog("OH WELL!");
			return (1 / (int)B_stats[i].name);
		}
		strcpy(B_stats[i].name, s);
		slog("Initializing %s", B_stats[i].name);
		sj_get_integer_value(sj_object_get_value(building, "id"), &B_stats[i].id);
		sj_get_integer_value(sj_object_get_value(building, "mode"), &B_stats[i].mode);
		sj_get_integer_value(sj_object_get_value(building, "maxHealth"), &B_stats[i].maxHealth);
		sj_get_integer_value(sj_object_get_value(building, "cost"), &B_stats[i].cost);
		sj_get_integer_value(sj_object_get_value(building, "size"), &B_stats[i].size);
		sj_get_integer_value(sj_object_get_value(building, "damage"), &B_stats[i].damage);
		sj_get_integer_value(sj_object_get_value(building, "tickstofire"), &B_stats[i].tickstofire);
		sj_get_float_value(sj_object_get_value(building, "range"), &B_stats[i].range);
		sj_get_float_value(sj_object_get_value(building, "vulnerability"), &B_stats[i].vulnerability);
		B_stats[i].range *= 5;
		slog("Initialized %s with MaxHealth %i Damage %i TicksToFire %i Range %f, Vulnerability %f", B_stats[i].name, B_stats[i].maxHealth, B_stats[i].damage, B_stats[i].tickstofire, B_stats[i].range, B_stats[i].vulnerability);
		i++;
		sj_free(building);
	}
}

BuildingInfo* B_stats_get(Building id, Uint8 mode) {
	return &B_stats[3 * id + mode];
}

void B_init_buildinginfos(SJson* json, Building id, Uint8 mode) {
	int i = 3 * id + mode;
	SJson* building = sj_array_get_nth(json, i);
	slog("id %i mode %i cost %i tickstofire %i maxHealth %i damage %i range %i vulnerability %i",
		sj_get_integer_value(sj_object_get_value(building, "id"), &B_stats[i].id),
		sj_get_integer_value(sj_object_get_value(building, "mode"), &B_stats[i].mode),
		sj_get_integer_value(sj_object_get_value(building, "cost"), &B_stats[i].cost),
		sj_get_integer_value(sj_object_get_value(building, "tickstofire"), &B_stats[i].tickstofire),
		sj_get_integer_value(sj_object_get_value(building, "maxHealth"), &B_stats[i].maxHealth),
		sj_get_integer_value(sj_object_get_value(building, "damage"), &B_stats[i].damage),
		sj_get_float_value(sj_object_get_value(building, "range"), &B_stats[i].range),
		sj_get_float_value(sj_object_get_value(building, "vulnerability"), &B_stats[i].vulnerability));
	slog("Initialized %s MaxHealth %i", B_stats[i].name, B_stats[i].maxHealth);
	if (id == ROCKETS || id == WIZARD_TOWER || id == INFERNO_TOWER) {
		B_stats[i].flags |= BF_AIR;
	}
	if (id == CANNON || id == MORTAR || id == WIZARD_TOWER || id == INFERNO_TOWER) {
		B_stats[i].flags |= BF_GROUND;
	}

	sj_object_free(building);
}


void B_apply_upgrade(Entity* building, Uint8 mode) {
	if (mode == 0) return;
	BuildingInfo* info = (BuildingInfo*)building->customData;
	BuildingInfo* newinfo = B_stats_get(info->id,mode);
	if (newinfo->cost > saveData->gold) {
		slog("Come back when you're a little, mmm, richer, okay?");
		game.state = Day;
		return;
	}
	else {
		saveData->gold -= newinfo->cost;
		building->customData = newinfo;
		building->health = newinfo->maxHealth; 
		char modelpath[100];
		sprintf(modelpath,"models/%s.obj", newinfo->name);

		slog("Loading %s", modelpath);

		char texturepath[100];
		sprintf(texturepath,"models/textures/%s.png", newinfo->name);

		building->model = gf3d_model_load_full(modelpath, texturepath);
		slog("Upgraded to %s", info->name);
		game.state = Day;
	}

}

void B_try_buy(Building id) {
	BuildingInfo* info = B_stats_get(id,0);
	if (info->cost > saveData->gold) {
		slog("Come back when you're a little, mmm, richer, okay?");
		return;
	}
	else{
		saveData->gold -= info->cost;
		if (id == LEY || id == MINE) {
			info->cost *= 4;
		}
		slog("Try to place the %s", info->name);
		B_choose_location(info);
	}
}

Entity* B_spawn(Vector3D position, Building id) {
	Entity* b = building_new();
	if (!b) {
		slog("Unable to spawn new building");
		return NULL;
	}

	BuildingInfo* info= B_stats_get(id, 0);

	vector3d_copy(b->position, position);

	b->health = info->maxHealth;

	slog("Health %i", info->maxHealth);
	slog("id %i", info->id);
	slog("Damage %i", info->damage);
	slog("Cost %i", info->cost);

	b->state = ES_idle;

	b->customData = info;

	b->think = B_think; //Draw with radius
	b->damage = B_damage;

	if (info->id == HALL || info->id == HUT || info->id == LAB || info->id == WALL) {
		b->think = NULL;
		b->info = ENTTYPE_RESOURCE;
	}
	else if (info->id == MINE || info->id == LEY) {
		b->info = ENTTYPE_RESOURCE;
	}
	else {
		b->info = ENTTYPE_BUILDING;
	}

	b->draw = B_draw;

	b->bounds = gfc_box(position.x - info->size / 2.0f, position.y - info->size / 2.0f, 0, info->size, info->size, info->size);


	char modelpath[100];
	sprintf(modelpath, "models/%s.obj", info->name);

	slog("Loading %s", modelpath);

	char texturepath[100];
	sprintf(texturepath, "models/textures/%s.png", info->name);

	b->model = gf3d_model_load_full(modelpath, texturepath);
	saveData->totalBuildings[info->id]++;
	slog("Placed building %s at %f, %f", info->name, b->position.x, b->position.z);
	game.name = 0;
	game.state = Day;
	return b;
}

void B_break(Entity* building) {
	slog("Ow oof I breaked!!1!");
	building->state = ES_dead;
	building->color.a = 0.5f;
	BuildingInfo* info=(BuildingInfo*)building->customData;
	if (info->id == MORTAR && info->mode == MORTAR_BLOWUP) {
		B_mortar_explode(building);
	}
	else if (info->id == INFERNO_TOWER && info->mode == MELTDOWN_INFERNO) {
		B_inferno_meltdown(building);
	}
	else if (info->id == HALL) {
		B_break(building);
	}
}

void B_try_rebuild(Entity* building) {

}