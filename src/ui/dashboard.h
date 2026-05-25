#pragma once

#include "core/types.h"

namespace grip {

void render_dashboard(SimState& state, MotorParams& motor_params,
                      PhysicsParams& physics_params, float fps);

} // namespace grip
