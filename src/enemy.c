#include "simple_logger.h"
#include "simple_json.h"
#include "enemy.h"
#include "save.h"

void M_think(Entity* self) {
	if (self->health == 0) {
		slog("Died");
		self->onDeath(self);
	}
	if (self->info & STATUS_FROZEN && SDL_GetTicks() < self->ticksSinceStatus) {
		slog("Frozen");
		return;
	}
	EnemyInfo* info = (EnemyInfo*)self->customData;
	if (self->info & STATUS_BURNED && SDL_GetTicks() < self->ticksSinceStatus && (self->ticksSinceStatus - SDL_GetTicks())%256<5) {
		self->damage(self, info->maxHealth / 30, NULL);
	}
	if (!self->target || self->target->state == ES_dead) {
		self->target = M_find_nearest(self);
		slog("Found target");
	}
	if (!self->target) {
		slog("No target found");
		M_die(self);
		return;
	}
	if (info->range == 0 && vector3d_distance_between_less_than(self->position,self->target->position,3.0f)) {
		if (SDL_GetTicks() - self->fireTimer >= info->tickstofire) {
			slog("Melee");
			self->state = ES_attack;
			slog("Did %i damage",M_melee(self, self->target));
		}
	}
	else if (vector3d_distance_between_less_than(self->position, self->target->position, info->range)) {
			self->state = ES_attack; 
			if (SDL_GetTicks() - self->fireTimer >= info->tickstofire) {
				M_fire(self, self->target);
			}
	}
	else {
		self->state = ES_hunt;
	}
}

void M_update(Entity* self) {
	if (self->state != ES_hunt) {
		return;
	}
	else {
		float speedfactor = self->info & STATUS_SLUDGE ? 0.5f : 1.0f;
		EnemyInfo* info = (EnemyInfo*)self->customData;
		Vector3D mov = vector3d_move_toward(self->position, self->target->position, info->baseSpeed * speedfactor/8);
		vector3d_add(self->position, self->position, mov);
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
	attacker->fireTimer = SDL_GetTicks();
	return (int)info->damage * factor;
}

void M_die(Entity* enemy) {
	EnemyInfo* info = (EnemyInfo*)enemy->customData;
	if (info->id == GOLEM) {
		M_golem_die(enemy);
	}
	else {
		slog("Killed a %s", info->name);
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
	slog("%i", m->health);
	m->bounds = gfc_box(position.x - info->size / 2.0f, position.y - info->size / 2.0f, position.z - info->size / 2.0f, info->size, info->size, info->size);
	char modelpath[100];
	strcpy(modelpath, "models/");
	strcat(modelpath, name);
	strcat(modelpath, ".obj");

	slog("Loading %s", modelpath);

	char texturepath[100];
	strcpy(texturepath, "models/textures/");
	strcat(texturepath, name);
	strcat(texturepath, ".png");

	m->model = gf3d_model_load_full(modelpath, texturepath);
	m->think = M_think;
	m->damage = M_damage;
	m->update = M_update;
	m->onDeath = M_die;
	m->info = ENTTYPE_ENEMY;

	if (info->id == BAT || info->id == DRAGON) {
		m->info |= ENTTYPE_ENEMY_AIR;
	}
	else {
		m->info |= ENTTYPE_ENEMY_GRD;
	}

	m->target = NULL;

	game.weight += info->weight;
	return m;
}

void M_damage(Entity* self, float damage, Entity* inflictor) {
	if (self->health <= damage) {
		self->health = 0;
		self->onDeath(self);
	}
	else {
		self->health -= (int)damage;
	}
}

void M_statmap_init() {
	M_statmap = gfc_hashmap_new();
	EnemyInfo* archer = (EnemyInfo*)gfc_allocate_array(sizeof(EnemyInfo),1);
	EnemyInfo* buster = (EnemyInfo*)gfc_allocate_array(sizeof(EnemyInfo), 1);
	EnemyInfo* golem = (EnemyInfo*)gfc_allocate_array(sizeof(EnemyInfo), 1);
	EnemyInfo* golemite = (EnemyInfo*)gfc_allocate_array(sizeof(EnemyInfo), 1);
	EnemyInfo* bat = (EnemyInfo*)gfc_allocate_array(sizeof(EnemyInfo), 1);
	EnemyInfo* dragon = (EnemyInfo*)gfc_allocate_array(sizeof(EnemyInfo), 1);
	//EnemyInfo* shambler = (EnemyInfo*)malloc(sizeof(EnemyInfo));

	SJson* json = sj_load("config/enemies.json");
	M_init_enemyinfo(json, "archer", archer);
	M_init_enemyinfo(json, "buster", buster);
	M_init_enemyinfo(json, "golem", golem);
	M_init_enemyinfo(json, "golemite", golemite);
	M_init_enemyinfo(json, "bat", bat);
	M_init_enemyinfo(json, "dragon", dragon);
	//M_init_enemyinfo(json, "shambler", shambler);
}

void M_init_enemyinfo(SJson* json, char* name, EnemyInfo* mptr) {
	SJson* building = sj_object_get_value(json, name);
	mptr->name = name;
	slog("Initializing %s", name);
	sj_get_integer_value(sj_object_get_value(building, "id"), &mptr->id);
	sj_get_integer_value(sj_object_get_value(building, "weight"), &mptr->weight);
	sj_get_float_value(sj_object_get_value(building, "baseSpeed"), &mptr->baseSpeed);
	sj_get_integer_value(sj_object_get_value(building, "tickstofire"), &mptr->tickstofire);
	sj_get_integer_value(sj_object_get_value(building, "maxHealth"), &mptr->maxHealth);
	sj_get_float_value(sj_object_get_value(building, "damage"), &mptr->damage);
	sj_get_float_value(sj_object_get_value(building, "range"), &mptr->range);
	if (mptr->id == DRAGON || mptr->id == BAT) {
		mptr->flags|=EF_AIR;
	}
	else if (mptr->id == BUSTER) {
		mptr->flags |= EF_TARGET_DEFENSE;
	}
	mptr->flags |= EF_ENEMY;
	slog("%s has health %i baseSpeed %f damage %f", name, mptr->maxHealth, mptr->baseSpeed, mptr->damage);
	gfc_hashmap_insert(M_statmap, name, mptr);
	sj_object_free(building);
}