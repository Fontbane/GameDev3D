#include "building.h"

void B_mortar_explode(Entity* building) {
	Entity* rocket = building_new();
	Sphere sphere = {
		.x = building->position.x,
		.y = building->position.y,
		.z = building->position.z,
		.r = 0.1f
	};
	rocket->think = B_explosion_think;
	rocket->position = building->position;
	rocket->draw = B_explosion_draw;
	rocket->state = ES_notarget;
	rocket->info = 1200;

	rocket->customData = &sphere;
	rocket->fireTimer = SDL_GetTicks();
}

void B_explosion_think(Entity* self) {
	Sphere* sphere = (Sphere*)self->customData;
	if ((SDL_GetTicks() - self->fireTimer) % 128 == 0) {
		sphere->r = (SDL_GetTicks() - self->fireTimer) / 512.0f;
	}
	if (sphere->r > 9) entity_free(self);
	sphere_damage(*sphere, self->info);
}

void B_explosion_draw(Entity* self) {
	Sphere* sphere = (Sphere*)self->customData;
	gf3d_draw_sphere_solid(*sphere, self->position, vector3d(0, 0, 0), vector3d(1, 1, 1), gfc_color(1, 0, 0, 0.5), gfc_color(1, 1, 1, 1));
}