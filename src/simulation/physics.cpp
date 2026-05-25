#include "physics.h"
#include <algorithm>
#include <cmath>

namespace grip {

PhysicsOutput physics_step(float flow_Lmin, float current_gauge_kpa,
                           float current_volume_l, float dt,
                           const PhysicsParams& params) {
    // Volume changes elastically with pressure
    float volume_l = params.V0 + params.compliance * current_gauge_kpa;
    volume_l = std::clamp(volume_l, 0.1f, 2.0f);
    float V = volume_l / 1000.0f;

    float P_abs = (current_gauge_kpa + params.P_atm) * 1000.0f;
    float Q = flow_Lmin / 1000.0f / 60.0f;

    float dP_dt = P_abs * Q / V;

    float new_P_abs = P_abs + dP_dt * dt;

    float min_P_abs = 10.0f * 1000.0f;
    float max_P_abs = 200.0f * 1000.0f;
    new_P_abs = std::clamp(new_P_abs, min_P_abs, max_P_abs);

    float new_gauge_kpa = new_P_abs / 1000.0f - params.P_atm;

    return {new_gauge_kpa, volume_l};
}

} // namespace grip
