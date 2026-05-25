#include "motor.h"
#include <cmath>

namespace grip {

MotorOutput motor_step(float pwm, float gauge_kpa, const MotorParams& params) {
    float pwm_mag = std::abs(pwm);
    float pwm_ratio = pwm_mag / 100.0f;
    float direction = (pwm >= 0.0f) ? 1.0f : -1.0f;

    if (pwm_mag < 0.01f) {
        direction = 0.0f;
    }

    float voltage = direction * pwm_ratio * params.V_supply;

    float load_current = params.k_torque * std::abs(gauge_kpa);
    float current = params.I_no_load + load_current;

    float power = std::abs(voltage) * current;
    if (pwm_mag < 0.01f) {
        power = 0.0f;
    }

    float flow = direction * params.flow_max * pwm_ratio;

    return {voltage, current, power, flow};
}

} // namespace grip
