#include "building.h"
#include "gf3d_draw.h"

void B_rocket_fire(Vector3D position, Uint16 damage) {
	Entity* rocket = building_new();
	Sphere sphere=gfc_sphere(position.x,position.y,position.z,0.1f);
	rocket->think = B_rocket_think;
	rocket->position = position;
	rocket->draw = B_rocket_draw;
	rocket->team = damage;

	rocket->customData = &sphere;
	rocket->fireTimer = SDL_GetTicks();
}

void B_rocket_think(Entity* self) {
	Sphere* sphere = (Sphere*)self->customData;
	if ((SDL_GetTicks() - self->fireTimer) % 128 == 0) {
		sphere->r = (SDL_GetTicks() - self->fireTimer) / 512.0f;
	}
	if (sphere->r > 3) entity_free(self);
	sphere_damage(*sphere, self->team, 0);
}

void B_rocket_draw(Entity* self) {
	Sphere* sphere = (Sphere*)self->customData;
	gf3d_draw_sphere_solid(*sphere, self->position, vector3d(0, 0, 0), vector3d(1, 1, 1), gfc_color(1, 0, 0, 0.5), gfc_color(1, 1, 1, 1));
}