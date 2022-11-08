#include "simple_logger.h"

#include "building.h"

BuildingInfo b_cannon = {
    .id=CANNON,
    .range=8,
    .vulnerability=0,
    .damage=5,
    .tickstofire=500,
    .maxHealth=150,
    .cost=100,
    .targets=1,
    .flags=BF_DEFENSE|BF_GROUND
};

/*Entity* cannon_entify(BuildingData data) {
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("Not enough memory to entify this cannon!");
        return NULL;
    }

    ent->customData = &data;
    ent->model = gf3d_model_load("b_cannon");
    ent->think = cannon_think;
    building_entify(ent, b_cannon);
    return ent;
}*/

void cannon_think(Entity* self) {

}