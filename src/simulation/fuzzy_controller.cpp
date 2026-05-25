#include "fuzzy_controller.h"
#include <algorithm>

namespace grip {

FuzzyController::FuzzyController() = default;

float FuzzyController::triangular(float x, float a, float b, float c) {
    if (x <= a || x >= c) return 0.0f; if (x <= b) return (x - a) / (b - a);
    return (c - x) / (c - b);
}

float FuzzyController::trapezoidal(float x, float a, float b, float c, float d) {
    if (x <= a || x >= d) return 0.0f;
    if (x >= b && x <= c) return 1.0f;
    if (x < b) return (x - a) / (b - a);
    return (d - x) / (d - c);
}

void FuzzyController::fuzzify_error(float e, float mems[5]) const {
    mems[0] = trapezoidal(e, -100.0f, params_.e_nb, params_.e_nb, params_.e_ns);
    mems[1] = triangular(e, params_.e_nb, params_.e_ns, params_.e_ze);
    mems[2] = triangular(e, params_.e_ns, params_.e_ze, params_.e_ps);
    mems[3] = triangular(e, params_.e_ze, params_.e_ps, params_.e_pb);
    mems[4] = trapezoidal(e, params_.e_ps, params_.e_pb, params_.e_pb, 100.0f);
}

void FuzzyController::fuzzify_derror(float de, float mems[5]) const {
    mems[0] = trapezoidal(de, -100.0f, params_.de_nb, params_.de_nb, params_.de_ns);
    mems[1] = triangular(de, params_.de_nb, params_.de_ns, params_.de_ze);
    mems[2] = triangular(de, params_.de_ns, params_.de_ze, params_.de_ps);
    mems[3] = triangular(de, params_.de_ze, params_.de_ps, params_.de_pb);
    mems[4] = trapezoidal(de, params_.de_ps, params_.de_pb, params_.de_pb, 100.0f);
}

float FuzzyController::compute(float error_kpa, float error_rate_kpa_s) {
    float mu_e[5], mu_de[5];
    fuzzify_error(error_kpa, mu_e);
    fuzzify_derror(error_rate_kpa_s, mu_de);

    float rule_strength[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            float strength = std::min(mu_e[i], mu_de[j]);
            int out_idx = RULES[i][j];
            rule_strength[out_idx] = std::max(rule_strength[out_idx], strength);
        }
    }

    float numerator = 0.0f;
    float denominator = 0.0f;
    for (int k = 0; k < 5; ++k) {
        numerator += rule_strength[k] * OUTPUT_CENTERS[k];
        denominator += rule_strength[k];
    }

    if (denominator < 1e-6f) {
        return 0.0f;
    }

    return numerator / denominator;
}

} // namespace grip
