#pragma once

namespace grip {

class FuzzyController {
public:
    struct Params {
        float e_nb = -12.0f;
        float e_ns = -6.0f;
        float e_ze = 0.0f;
        float e_ps = 6.0f;
        float e_pb = 12.0f;

        float de_nb = -6.0f;
        float de_ns = -3.0f;
        float de_ze = 0.0f;
        float de_ps = 3.0f;
        float de_pb = 6.0f;
    };

    FuzzyController();

    float compute(float error_kpa, float error_rate_kpa_s);

    const Params& getParams() const { return params_; }
    void setParams(const Params& p) { params_ = p; }

private:
    static float triangular(float x, float a, float b, float c);
    static float trapezoidal(float x, float a, float b, float c, float d);
    void fuzzify_error(float e, float mems[5]) const;
    void fuzzify_derror(float de, float mems[5]) const;

    // e\de     NB  NS  Z   PS  PB
    // NB       0   0   0   0   0
    // NS       0   0   0   0   1
    // Z        0   0   0   1   2
    // PS       2   2   3   3   4
    // PB       3   4   4   4   4
    static constexpr int RULES[5][5] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 2},
        {2, 2, 3, 3, 4},
        {3, 4, 4, 4, 4},
    };

    static constexpr float OUTPUT_CENTERS[5] = {0.0f, 25.0f, 50.0f, 75.0f, 100.0f};

    Params params_;
};

} // namespace grip
