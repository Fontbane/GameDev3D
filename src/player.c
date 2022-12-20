#include "simple_logger.h"
#include "gfc_types.h"
#include "gf2d_mouse.h"
#include "gf3d_draw.h"
#include "gf3d_vgraphics.h"

#include "gf3d_camera.h"
#include "player.h"

static int thirdPersonMode = 0;
void player_think(Entity *self);
void player_update(Entity *self);

Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->think = player_think;
    ent->update = player_update;
    vector3d_copy(ent->position,position);
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = GFC_PI;
    ent->draw = player_draw;
    ent->info = ENTTYPE_RESOURCE;
    return ent;
}

void player_draw(Entity* self)
{
    /*Edge3D raycast = gf2d_mouse_get_cast_ray();
    game.location_chosen = raycast.b;
        slog("%f,%f,%f to %f,%f,%f", raycast.a.x, raycast.a.y, raycast.a.z, raycast.b.x, raycast.b.y, raycast.b.z);
    gf3d_draw_circle(gfc_circle(0, 2, 1), game.location_chosen, vector3d(0,0,0),vector3d(1,1,1),gfc_color(1,0,0,0.5f));
    gf3d_draw_edge_3d(raycast, vector3d(0, 0, 0), vector3d(0, 0, 0), vector3d(1, 1, 1), 0.025, gfc_color(0, 1, 0, 1));*/
    gf3d_draw_circle(gfc_circle(0, 0, 1), game.location_chosen, vector3d(0, 0, 0), vector3d(1, 1, 1), gfc_color(1, 0, 0, 0.5f));
}

void player_think(Entity *self)
{
    Vector3D forward = {0};
    Vector3D right = { 0 };
    Vector2D w, mouse;
    int mx, my;
    SDL_GetRelativeMouseState(&mx, &my);
    const Uint8* keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = w.x;
    right.y = w.y;
    if (keys[SDL_SCANCODE_B])
    {
        vector3d_add(self->position, self->position, forward);
    }
    if (keys[SDL_SCANCODE_7])
    {
        vector3d_add(self->position, self->position, -forward);
    }
    if (keys[SDL_SCANCODE_N])
    {
        vector3d_add(self->position, self->position, right);
    }
    if (keys[SDL_SCANCODE_8])
    {
        vector3d_add(self->position, self->position, -right);
    }
    //if (keys[SDL_SCANCODE_SPACE])self->position.z += 1;
    if (keys[SDL_SCANCODE_PERIOD])self->position.z -= 1;
    if (keys[SDL_SCANCODE_9])self->rotation.y -= 0.0050;
    if (keys[SDL_SCANCODE_0])self->rotation.y += 0.0050;
    if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050;
    if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050;
    if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050;

    /*if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);*/

    if (keys[SDL_SCANCODE_F3])
    {
        thirdPersonMode = !thirdPersonMode;
        self->hidden = !self->hidden;
    }
    game.location_chosen = gf2d_mouse_screen_to_world();
    
}

void player_update(Entity *self)
{
    Vector3D forward = {0};
    Vector3D position;
    Vector3D rotation;
    Vector2D w;

    if (!self)return;

    vector3d_copy(position, self->position);
    vector3d_copy(rotation, self->rotation);
    if (thirdPersonMode)
    {
        position.z += 100;
        rotation.x += M_PI * 0.125;
        w = vector2d_from_angle(self->rotation.z);
        forward.x = w.x * 100;
        forward.y = w.y * 100;
        vector3d_add(position, position, -forward);
    }
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(rotation);
}

/*eol@eof*/
