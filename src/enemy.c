#include "simple_logger.h"
#include "simple_json.h"
#include "enemy.h"
#include "save.h"

void M_think(Entity* self) {
	if (self->health == 0) {
		self->onDeath(self);
	}
	if (self->info & STATUS_FROZEN && SDL_GetTicks() < self->ticksSinceStatus) {
		return;
	}
	EnemyInfo* info = (EnemyInfo*)self->customData;
	if (self->info & STATUS_BURNED && SDL_GetTicks() < self->ticksSinceStatus && (self->ticksSinceStatus- SDL_GetTicks())%256==0) {
		self->damage(self, info->maxHealth / 30, NULL);
	}
	if (!self->target || self->target->state == ES_dead) {
		self->target = M_find_nearest(self);
	}
	if (!self->target) {
		slog("No target found");
		M_die(self);
	}
	if (info->range == 0 && gfc_box_overlap(self->bounds, self->target->bounds)) {
		if (SDL_GetTicks() - self->fireTimer >= info->tickstofire)
			M_melee(self, self->target);
	}
	else if (vector3d_distance_between_less_than(self->position, self->target->position, info->range)) {
		if (SDL_GetTicks() - self->fireTimer >= info->tickstofire)
			M_fire(self, self->target);
	}
	else {
		float speedfactor = self->info & STATUS_SLUDGE ? 0.5 : 1.0;
		self->position = vector3d_move_toward(self->position, self->target->position, info->baseSpeed*speedfactor);
	}
}

int M_fire(Entity* attacker, Entity* target) {
	return 0;
}

int M_melee(Entity* attacker, Entity* target) {
	EnemyInfo* info = (EnemyInfo*)attacker->customData;
	float factor = info->id == BUSTER && target->health < 100 ? 1.5 : 1.0;
	if (attacker->info & STATUS_SLUDGE) {
		factor *= 0.5;
	}
	target->damage(target, info->damage * factor, attacker);
	return (int)info->damage * factor;
}

void M_die(Entity* enemy) {
	EnemyInfo* info = (EnemyInfo*)enemy->customData;
	if (info->id == GOLEM) {
		M_golem_die(enemy);
	}
	else {
		saveData->gold += info->weight * info->weight * 4;
		saveData->elixir += info->weight * info->weight * 4;
		saveData->enemiesKilled++;
		entity_free(enemy);
	}
}

Entity* M_spawn(Vector3D position, char* name){
	Entity* m = entity_new();
	if (!m) {
		slog("Not enough memory for a new enemy!");
		return NULL;
	}

	m->position = position;

	m->customData = gfc_hashmap_get(M_statmap, name);
	if (!m->customData) {
		slog("Data for enemy %s not found", name);
		entity_free(m);
		return NULL;
	}
	EnemyInfo* info = (EnemyInfo*)m->customData;
	m->health = info->maxHealth;
	Box bounds = {
		.x = position.x - info->size / 2.0f,
		.y = position.y - info->size / 2.0f,
		.z = position.z - info->size / 2.0f,

		.w = info->size,
		.h = info->size,
		.d = info->size
	};
	m->bounds = bounds;
	m->model = gf3d_model_load(name);
	m->onDeath = M_die;
}

void M_damage(Entity* self, float damage, Entity* inflictor) {
	if (self->health < damage) {
		self->health = 0;
	}
	else {
		self->health -= (int)damage;
	}
	if (self->health == 0) {
		self->onDeath(self);
	}
}

void M_statmap_init() {
	M_statmap = gfc_hashmap_new();
	EnemyInfo archer = { 0 };
	EnemyInfo buster = { 0 };
	EnemyInfo golem = { 0 };
	EnemyInfo golemite = { 0 };
	EnemyInfo bat = { 0 };
	EnemyInfo dragon = { 0 };
	EnemyInfo shambler = { 0 };

	SJson* json = sj_load("config/enemies.json");
	M_init_enemyinfo(json, "archer", &archer);
	M_init_enemyinfo(json, "buster", &buster);
	M_init_enemyinfo(json, "golem", &golem);
	M_init_enemyinfo(json, "golemite", &golemite);
	M_init_enemyinfo(json, "bat", &bat);
	M_init_enemyinfo(json, "dragon", &dragon);
	M_init_enemyinfo(json, "shambler", &shambler);
}

void M_init_enemyinfo(SJson* json, char* name, EnemyInfo* mptr) {
	SJson* building = sj_object_get_value(json, name);
	mptr->name = name;
	sj_get_integer_value(sj_object_get_value(building, "id"), &mptr->id);
	sj_get_integer_value(sj_object_get_value(building, "weight"), &mptr->weight);
	sj_get_integer_value(sj_object_get_value(building, "baseSpeed"), &mptr->baseSpeed);
	sj_get_integer_value(sj_object_get_value(building, "tickstofire"), &mptr->tickstofire);
	sj_get_integer_value(sj_object_get_value(building, "maxHealth"), &mptr->maxHealth);
	sj_get_integer_value(sj_object_get_value(building, "damage"), &mptr->damage);
	sj_get_float_value(sj_object_get_value(building, "range"), &mptr->range);
	if (mptr->id == DRAGON || mptr->id == BAT) {
		mptr->flags&=EF_AIR;
	}
	else if (mptr->id == BUSTER) {
		mptr->flags &= EF_TARGET_DEFENSE;
	}
	mptr->flags &= EF_ENEMY;
	gfc_hashmap_insert(B_statmap, name, mptr);
	sj_free(building);
}