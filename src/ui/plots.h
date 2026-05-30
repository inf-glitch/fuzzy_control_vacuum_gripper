#pragma once

#include "core/types.h"

namespace grip {

struct PlotData {
    static constexpr int CAPACITY = 3600;

    float times[CAPACITY];
    float pressures[CAPACITY];
    float powers[CAPACITY];
    float flows[CAPACITY];
    int count = 0;

    void push(float t, float p, float pw, float f);
    void clear();
};

void render_plots(const SimState& state, PlotData& data,
                  bool compare_mode, PlotData& compare_data);

} // namespace grip
