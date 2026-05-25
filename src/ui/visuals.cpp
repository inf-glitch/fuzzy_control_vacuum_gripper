#include "visuals.h"
#include "imgui.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace grip {

static float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

static ImVec4 lerp_color(const ImVec4& a, const ImVec4& b, float t) {
    return ImVec4(
        lerp(a.x, b.x, t),
        lerp(a.y, b.y, t),
        lerp(a.z, b.z, t),
        lerp(a.w, b.w, t)
    );
}

void render_visuals(const SimState& state) {
    if (!ImGui::Begin("Gripper View", nullptr,
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {
        ImGui::End();
        return;
    }

    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetCursorScreenPos();
    ImVec2 win_size = ImGui::GetContentRegionAvail();

    float cx = win_pos.x + win_size.x * 0.5f;
    float cy = win_pos.y + win_size.y * 0.5f;

    float base_radius = std::min(win_size.x, win_size.y) * 0.35f;

    // Balloon size changes with volume
    float volume_ratio = state.volume_l / 0.5f;
    float radius = base_radius * std::cbrt(volume_ratio);
    radius = std::clamp(radius, base_radius * 0.5f, base_radius * 1.5f);

    // Compute color: blue (soft) -> red (jammed)
    ImVec4 soft_color(0.3f, 0.6f, 1.0f, 0.9f);
    ImVec4 firm_color(1.0f, 0.8f, 0.2f, 0.9f);
    ImVec4 solid_color(1.0f, 0.2f, 0.2f, 0.9f);

    ImVec4 balloon_color;
    if (state.rigidity < 0.35f) {
        float t = state.rigidity / 0.35f;
        balloon_color = lerp_color(soft_color, firm_color, t);
    } else if (state.rigidity < 0.7f) {
        float t = (state.rigidity - 0.35f) / 0.35f;
        balloon_color = lerp_color(firm_color, solid_color, t);
    } else {
        balloon_color = solid_color;
    }

    ImU32 balloon_u32 = ImGui::ColorConvertFloat4ToU32(balloon_color);

    // Draw tube
    float tube_top = win_pos.y;
    float tube_bottom = cy - radius * 0.6f;
    draw->AddLine(ImVec2(cx, tube_top), ImVec2(cx, tube_bottom),
                  IM_COL32(120, 120, 120, 255), 4.0f);

    // Draw balloon (filled circle)
    draw->AddCircleFilled(ImVec2(cx, cy), radius, balloon_u32, 64);

    // Balloon outline
    ImU32 outline = IM_COL32(200, 200, 200, 180);
    draw->AddCircle(ImVec2(cx, cy), radius, outline, 64, 2.0f);

    // Draw granular particles inside
    unsigned int seed = 42;
    int num_particles = 40;
    float particle_radius = radius * 0.06f;
    for (int i = 0; i < num_particles; ++i) {
        seed = seed * 1103515245u + 12345u;
        float angle = (seed % 1000) / 1000.0f * 2.0f * 3.14159f;
        seed = seed * 1103515245u + 12345u;
        float dist = (seed % 1000) / 1000.0f * radius * 0.7f;

        float px = cx + std::cos(angle) * dist;
        float py = cy + std::sin(angle) * dist;

        ImU32 particle_color = IM_COL32(180, 140, 80, 200);
        draw->AddCircleFilled(ImVec2(px, py), particle_radius, particle_color, 12);
    }

    // Info overlay
    char buf[128];
    std::snprintf(buf, sizeof(buf), "P: %.1f kPa  |  V: %.2f L  |  Rigidity: %.0f%%",
                  state.pressure_kpa, state.volume_l, state.rigidity * 100.0f);
    ImVec2 text_size = ImGui::CalcTextSize(buf);
    float text_x = cx - text_size.x * 0.5f;
    float text_y = cy + radius + 10.0f;
    draw->AddText(ImVec2(text_x, text_y), IM_COL32(220, 220, 220, 255), buf);

    ImGui::End();
}

} // namespace grip
