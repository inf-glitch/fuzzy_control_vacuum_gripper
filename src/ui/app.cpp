#include "app.h"
#include "ui/dashboard.h"
#include "ui/plots.h"
#include "ui/visuals.h"
#include "simulation/motor.h"
#include "simulation/physics.h"
#include "simulation/gripper.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"
#include "implot.h"

#include <SDL.h>
#include <algorithm>
#include <cmath>

namespace grip {

App::App() = default;

App::~App() {
    if (gl_ctx_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
        SDL_GL_DeleteContext(gl_ctx_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

bool App::init(int window_w, int window_h) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window_ = SDL_CreateWindow("Vacuum Jamming Gripper - Fuzzy Control Simulation",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               window_w, window_h,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window_) {
        return false;
    }

    gl_ctx_ = SDL_GL_CreateContext(window_);
    if (!gl_ctx_) {
        return false;
    }
    SDL_GL_MakeCurrent(window_, gl_ctx_);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(
        THIRD_PARTY_DIR "/imgui/misc/fonts/Roboto-Medium.ttf", 18.0f);

    if (!ImGui_ImplSDL2_InitForOpenGL(window_, gl_ctx_)) {
        return false;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        return false;
    }

    running_ = true;
    return true;
}

void App::simulate_step() {
    if (!state_.running || state_.paused) {
        return;
    }

    float dt = state_.dt * state_.sim_speed;
    state_.t += dt;

    float direction = (state_.setpoint_kpa > state_.pressure_kpa) ? 1.0f : -1.0f;
    float effective_pwm = direction * state_.pwm;

    if (state_.mode == ControlMode::AUTO) {
        float error = std::abs(state_.pressure_kpa - state_.setpoint_kpa);
        float error_rate = (error - prev_error_) / dt;
        prev_error_ = error;

        float fuzzy_pwm = fuzzy_ctrl_.compute(error, error_rate);
        state_.pwm = std::clamp(fuzzy_pwm, 0.0f, 100.0f);
        effective_pwm = direction * state_.pwm;
    }

    MotorOutput motor_out = motor_step(effective_pwm, state_.pressure_kpa, motor_params_);
    state_.voltage = motor_out.voltage;
    state_.current = motor_out.current;
    state_.power = motor_out.power;
    state_.flow = motor_out.flow;

    PhysicsOutput phys_out = physics_step(state_.flow, state_.pressure_kpa,
                                          state_.volume_l, dt, physics_params_);
    state_.pressure_kpa = phys_out.pressure_kpa;
    state_.volume_l = phys_out.volume_l;

    GripperOutput grip_out = gripper_update(state_.pressure_kpa);
    state_.rigidity = grip_out.rigidity;
    state_.grip_state = grip_out.grip_state;

    plot_data_.push(static_cast<float>(state_.t),
                    state_.pressure_kpa,
                    state_.power,
                    state_.flow);
}

void App::render_ui() {
    render_dashboard(state_, motor_params_, physics_params_, fps_);
    render_plots(state_, plot_data_);
    render_visuals(state_);
}

void App::frame() {
    double now = SDL_GetTicks64() / 1000.0;
    frame_count_++;
    if (now - last_fps_update_ >= 1.0) {
        fps_ = static_cast<float>(frame_count_ / (now - last_fps_update_));
        frame_count_ = 0;
        last_fps_update_ = now;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            running_ = false;
        }
        if (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_CLOSE) {
            running_ = false;
        }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    simulate_step();
    render_ui();

    ImGui::Render();
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window_);
}

} // namespace grip
