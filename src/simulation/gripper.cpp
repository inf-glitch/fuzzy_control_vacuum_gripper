#include "gripper.h"
#include <cmath>

namespace grip {

static constexpr float K_JAM = 0.12f;
static constexpr float P_THRESHOLD = 25.0f;

GripperOutput gripper_update(float gauge_kpa) {
    float abs_dp = std::abs(gauge_kpa);
    float rigidity = 1.0f / (1.0f + std::exp(-K_JAM * (abs_dp - P_THRESHOLD)));

    GripState state;
    if (rigidity < 0.35f) {
        state = GripState::SOFT;
    } else if (rigidity < 0.7f) {
        state = GripState::FIRM;
    } else {
        state = GripState::SOLID;
    }

    return {rigidity, state};
}

} // namespace grip
