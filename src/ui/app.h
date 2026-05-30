#pragma once

#include "core/types.h"
#include "ui/plots.h"
#include "simulation/fuzzy_controller.h"
#include "simulation/pd_controller.h"

struct SDL_Window;
typedef void* SDL_GLContext;

namespace grip {

class App {
public:
    App();
    ~App();

    bool init(int window_w, int window_h);
    bool running() const { return running_; }
    void frame();

private:
    void simulate_step();
    void render_ui();

    SDL_Window* window_ = nullptr;
    SDL_GLContext gl_ctx_ = nullptr;
    bool running_ = false;

    SimState state_;
    MotorParams motor_params_;
    PhysicsParams physics_params_;
    FuzzyController fuzzy_ctrl_;
    PdController pd_ctrl_;
    PlotData plot_data_;

    SimState compare_state_;
    PlotData compare_data_;

    float prev_error_ = 0.0f;
    float prev_error_compare_ = 0.0f;

    double last_fps_update_ = 0.0;
    int frame_count_ = 0;
    float fps_ = 0.0f;
};

} // namespace grip
