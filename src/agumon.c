
#include "simple_logger.h"
#include "agumon.h"


void agumon_think(Entity *self);

Entity *agumon_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no agumon for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("dino");
    ent->think = agumon_think;
    vector3d_copy(ent->position,position);
    return ent;
}


void agumon_think(Entity *self)
{
    if (!self)return;
    self->rotation.x += gfc_random()/2.0;
    float r = gfc_random();
    if (r > 0.5) {
        self->position.x -= (r - .5) / 2.0;
    }
    else {
        self->position.x += r/2.0;
    }
    r = gfc_random();
    if (r > 0.5) {
        self->position.y -= (r - .5) / 2.0;
    }
    else {
        self->position.y += r/2.0;
    }
    r = gfc_random();
    if (r > 0.5) {
        self->position.z -= (r - .5)/2.0;
    }
    else {
        self->position.z += r/2.0;
    }
}

/*eol@eof*/
