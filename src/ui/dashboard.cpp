#include "dashboard.h"
#include "imgui.h"
#include <cstdio>

namespace grip {

static void draw_grip_state_indicator(GripState state, float rigidity) {
    const char* label = "";
    ImVec4 color;
    switch (state) {
        case GripState::SOFT:
            label = "SOFT";
            color = ImVec4(0.3f, 0.6f, 1.0f, 1.0f);
            break;
        case GripState::FIRM:
            label = "FIRM";
            color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
            break;
        case GripState::SOLID:
            label = "SOLID";
            color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
            break;
    }
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextUnformatted(label);
    ImGui::PopStyleColor();

    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.0f%%", rigidity * 100.0f);
    ImGui::ProgressBar(rigidity, ImVec2(-1.0f, 0.0f), buf);
}

static void draw_readonly_value(const char* label, const char* fmt, float value,
                                const char* unit) {
    ImGui::TextUnformatted(label);
    ImGui::SameLine(120.0f);
    char buf[64];
    std::snprintf(buf, sizeof(buf), fmt, value);
    ImGui::Text("%s %s", buf, unit);
}

void render_dashboard(SimState& state, MotorParams& motor_params,
                      PhysicsParams& physics_params, float fps) {
    if (!ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("FPS: %.0f", fps);
        ImGui::Text("Time: %.1f s", state.t);

        if (!state.running) {
            if (ImGui::Button("Start", ImVec2(80.0f, 0.0f))) {
                state.running = true;
                state.paused = false;
            }
        } else {
            if (ImGui::Button(state.paused ? "Resume" : "Pause", ImVec2(80.0f, 0.0f))) {
                state.paused = !state.paused;
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop", ImVec2(80.0f, 0.0f))) {
                state.running = false;
            }
        }

        ImGui::SliderFloat("Speed", &state.sim_speed, 0.1f, 5.0f, "%.1fx");
    }

    if (ImGui::CollapsingHeader("Control", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* modes[] = {"Manual", "Auto"};
        int current = (state.mode == ControlMode::AUTO) ? 1 : 0;
        if (ImGui::Combo("Mode", &current, modes, 2)) {
            state.mode = (current == 1) ? ControlMode::AUTO : ControlMode::MANUAL;
        }

        if (state.mode == ControlMode::AUTO) {
            const char* ctrls[] = {"Fuzzy", "PD"};
            int ctrl_idx = (state.ctrl_type == ControllerType::PD) ? 1 : 0;
            if (ImGui::Combo("Controller", &ctrl_idx, ctrls, 2)) {
                state.ctrl_type = (ctrl_idx == 1) ? ControllerType::PD : ControllerType::FUZZY;
            }

            if (state.ctrl_type == ControllerType::PD) {
                ImGui::SliderFloat("Kp", &state.pd_params.Kp, 0.0f, 2.0f, "%.2f");
                ImGui::SliderFloat("Kd", &state.pd_params.Kd, 0.0f, 1.0f, "%.3f");
            }

            if (state.ctrl_type == ControllerType::FUZZY) {
                ImGui::Checkbox("Compare PD", &state.compare_mode);
            }

            ImGui::SliderFloat("Setpoint", &state.setpoint_kpa, -90.0f, 10.0f, "%.1f kPa");
        }

        ImGui::SliderFloat("PWM", &state.pwm, 0.0f, 100.0f, "%.0f%%");
    }

    if (ImGui::CollapsingHeader("Motor", ImGuiTreeNodeFlags_DefaultOpen)) {
        draw_readonly_value("Voltage", "%.2f", state.voltage, "V");
        draw_readonly_value("Current", "%.3f", state.current, "A");
        draw_readonly_value("Power", "%.2f", state.power, "W");
        draw_readonly_value("Flow", "%.2f", state.flow, "L/min");
    }

    if (ImGui::CollapsingHeader("Gripper", ImGuiTreeNodeFlags_DefaultOpen)) {
        draw_readonly_value("Pressure", "%.1f", state.pressure_kpa, "kPa");
        draw_readonly_value("Volume", "%.2f", state.volume_l, "L");
        ImGui::TextUnformatted("Grip:");
        ImGui::SameLine(120.0f);
        draw_grip_state_indicator(state.grip_state, state.rigidity);
    }

    if (ImGui::CollapsingHeader("Parameters")) {
        ImGui::SliderFloat("Motor V_supply", &motor_params.V_supply, 5.0f, 24.0f, "%.1f V");
        ImGui::SliderFloat("Motor I_no_load", &motor_params.I_no_load, 0.0f, 0.5f, "%.3f A");
        ImGui::SliderFloat("Motor R_winding", &motor_params.R_winding, 0.5f, 5.0f, "%.1f Ohm");
        ImGui::SliderFloat("Motor flow_max", &motor_params.flow_max, 1.0f, 20.0f, "%.1f L/min");
        ImGui::SliderFloat("Balloon V0", &physics_params.V0, 0.1f, 2.0f, "%.2f L");
    }

    ImGui::End();
}

} // namespace grip
