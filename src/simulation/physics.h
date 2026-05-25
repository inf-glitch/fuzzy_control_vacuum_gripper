#pragma once

#include "core/types.h"

namespace grip {

struct PhysicsOutput {
    float pressure_kpa;
    float volume_l;
};

PhysicsOutput physics_step(float flow_Lmin, float current_gauge_kpa,
                           float current_volume_l, float dt,
                           const PhysicsParams& params);

} // namespace grip
