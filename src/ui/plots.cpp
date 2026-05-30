#include "plots.h"
#include "imgui.h"
#include "implot.h"
#include <algorithm>
#include <cstring>

namespace grip {

void PlotData::push(float t, float p, float pw, float f) {
    if (count < CAPACITY) {
        times[count] = t;
        pressures[count] = p;
        powers[count] = pw;
        flows[count] = f;
        count++;
    } else {
        std::memmove(times, times + 1, (CAPACITY - 1) * sizeof(float));
        std::memmove(pressures, pressures + 1, (CAPACITY - 1) * sizeof(float));
        std::memmove(powers, powers + 1, (CAPACITY - 1) * sizeof(float));
        std::memmove(flows, flows + 1, (CAPACITY - 1) * sizeof(float));
        times[CAPACITY - 1] = t;
        pressures[CAPACITY - 1] = p;
        powers[CAPACITY - 1] = pw;
        flows[CAPACITY - 1] = f;
    }
}

void PlotData::clear() {
    count = 0;
}

void render_plots(const SimState& state, PlotData& data,
                  bool compare_mode, PlotData& compare_data) {
    if (!ImGui::Begin("Plots", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(4.0f, 4.0f));
    ImPlot::PushStyleColor(ImPlotCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

    // Pressure plot
    if (ImPlot::BeginPlot("Pressure", ImVec2(-1.0f, 160.0f), ImPlotFlags_NoTitle)) {
        ImPlot::SetupAxes("Time (s)", "kPa", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, state.t - 30.0, state.t, ImGuiCond_Always);
        const char* label = (state.ctrl_type == ControllerType::FUZZY) ? "Fuzzy" : "PD";
        ImPlot::PlotLine(label, data.times, data.pressures, data.count);
        if (compare_mode && compare_data.count > 0) {
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.5f, 0.1f, 1.0f));
            ImPlot::PlotLine("PD", compare_data.times, compare_data.pressures, compare_data.count);
        }
        ImPlot::EndPlot();
    }

    // Power plot
    if (ImPlot::BeginPlot("Power", ImVec2(-1.0f, 160.0f), ImPlotFlags_NoTitle)) {
        ImPlot::SetupAxes("Time (s)", "W", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, state.t - 30.0, state.t, ImGuiCond_Always);
        ImPlot::PlotLine("Power", data.times, data.powers, data.count);
        if (compare_mode && compare_data.count > 0) {
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.5f, 0.1f, 1.0f));
            ImPlot::PlotLine("PD", compare_data.times, compare_data.powers, compare_data.count);
        }
        ImPlot::EndPlot();
    }

    // Flow plot
    if (ImPlot::BeginPlot("Flow", ImVec2(-1.0f, 160.0f), ImPlotFlags_NoTitle)) {
        ImPlot::SetupAxes("Time (s)", "L/min", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, state.t - 30.0, state.t, ImGuiCond_Always);
        ImPlot::PlotLine("Flow", data.times, data.flows, data.count);
        ImPlot::EndPlot();
    }

    ImPlot::PopStyleColor();
    ImPlot::PopStyleVar();

    ImGui::End();
}

} // namespace grip
