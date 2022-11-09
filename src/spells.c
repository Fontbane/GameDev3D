#include "spells.h"
#include "simple_logger.h"
#include "save.h"
#include "gfc_shape.h";
#include "gf3d_draw.h"

void S_statmap_init() {
	S_statmap = gfc_hashmap_new();
	SpellInfo freeze = { 0 };
	SpellInfo heal = { 0 };
	SpellInfo sludge = { 0 };
	SpellInfo smite = { 0 };
	SpellInfo pyro = { 0 };

	SJson* json = sj_load("config/spells.json");
	S_init_spellinfo(json, "freeze_spell", &freeze);
	S_init_spellinfo(json, "heal_spell", &heal);
	S_init_spellinfo(json, "sludge_spell", &sludge);
	S_init_spellinfo(json, "smite_spell", &smite);
	S_init_spellinfo(json, "pyro_spell", &pyro);
	sj_free(json);
}
void S_init_spellinfo(SJson* json, char* name, SpellInfo* sptr) {
	SJson* building = sj_object_get_value(json, name);
	Uint32 c = 0;
	sptr->name = name;
	sj_get_integer_value(sj_object_get_value(building, "id"), &sptr->id);
	sj_get_integer_value(sj_object_get_value(building, "cost"), &sptr->cost);
	sj_get_integer_value(sj_object_get_value(building, "tickstofire"), &sptr->tickstofire);
	sj_get_integer_value(sj_object_get_value(building, "duration"), &sptr->duration);
	sj_get_float_value(sj_object_get_value(building, "radius"), &sptr->radius);
	sj_get_bool_value(sj_object_get_value(building, "effect_enemies"), &sptr->targetEnemies);
	sj_get_integer_value(sj_object_get_value(building, "color"), &c);
	sptr->color = gfc_color_hex(c);
	sptr->color.a = 0.75;
	gfc_hashmap_insert(S_statmap, name, sptr);
	sj_free(building);
}
void S_try_buy(Vector3D position, char* name) {
	SpellInfo* info = (SpellInfo*)gfc_hashmap_get(S_statmap, name);
	if (!info) {
		slog("could not find spell %s", name);
		return;
	}
	if (saveData->elixir < info->cost) {
		slog("Come back when you're a little, mmm, richer!");
		return;
	}
	S_spawn(position, name);
}
void S_spawn(Vector3D position, char* name) {
	Entity* s = entity_new();
	if (!s) {
		slog("could not summon spell");
		return;
	}
	s->customData = gfc_hashmap_get(S_statmap, name);
	if (!s->customData) {
		slog("could not find spell %s", name);
		entity_free(s);
		return;
	}
	s->state = ES_notarget;
	s->think = S_think;
	s->position = position;
	s->fireTimer = SDL_GetTicks();
}
void S_think(Entity* self) {
	SpellInfo* info = (SpellInfo*)self->customData;
	if (SDL_GetTicks() - self->fireTimer > info->duration) {
		entity_free(self);
		return;
	}
	else if ((info->id == HEAL_SPELL || info->id == SLUDGE_SPELL || info->id == PYRO_SPELL) 
		&& (SDL_GetTicks()-self->fireTimer)%info->tickstofire<info->duration) {
		S_effect(self);
		Circle c = {
			.x = 0,
			.y = 0,
			.r = info->radius
		};
		gf3d_draw_circle(c, self->position, vector3d(0, 0, 0), vector3d(1, 1, 1), info->color);
	}
}