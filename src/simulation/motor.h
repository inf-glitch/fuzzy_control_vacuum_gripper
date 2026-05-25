#pragma once

#include "core/types.h"

namespace grip {

struct MotorOutput {
    float voltage;
    float current;
    float power;
    float flow;
};

MotorOutput motor_step(float pwm, float gauge_kpa, const MotorParams& params);

} // namespace grip
