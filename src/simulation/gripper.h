#pragma once

#include "core/types.h"

namespace grip {

struct GripperOutput {
    float rigidity;
    GripState grip_state;
};

GripperOutput gripper_update(float gauge_kpa);

} // namespace grip
