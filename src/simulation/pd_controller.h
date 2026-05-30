#pragma once

#include "core/types.h"
#include <algorithm>
#include <cmath>

namespace grip {

struct PdController {
    float compute(float error_kpa, float error_rate_kpa_s, const PdParams& p) const {
        float raw = p.Kp * std::abs(error_kpa) + p.Kd * error_rate_kpa_s;
        return std::clamp(raw, 0.0f, 100.0f);
    }
};

} // namespace grip
