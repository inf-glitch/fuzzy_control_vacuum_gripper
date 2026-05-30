#pragma once

namespace grip {

enum class GripState { SOFT, FIRM, SOLID };

enum class ControlMode { MANUAL, AUTO };

enum class ControllerType { FUZZY, PD };

struct PdParams {
    float Kp = 0.8f;
    float Kd = 0.05f;
};

struct MotorParams {
    float V_supply = 12.0f;
    float I_no_load = 0.08f;
    float R_winding = 1.8f;
    float k_torque = 0.002f;
    float flow_max = 8.0f;

    static MotorParams defaults() { return MotorParams{}; }
};

struct PhysicsParams {
    float V0 = 0.5f;
    float compliance = 0.004f;
    float P_atm = 101.325f;

    static PhysicsParams defaults() { return PhysicsParams{}; }
};

struct SimState {
    double t = 0.0;
    float dt = 1.0f / 60.0f;

    ControlMode mode = ControlMode::MANUAL;
    ControllerType ctrl_type = ControllerType::FUZZY;
    PdParams pd_params;
    float pwm = 0.0f;
    float setpoint_kpa = -50.0f;

    float voltage = 0.0f;
    float current = 0.0f;
    float power = 0.0f;

    float flow = 0.0f;
    float pressure_kpa = 0.0f;
    float volume_l = 0.5f;

    float rigidity = 0.0f;
    GripState grip_state = GripState::SOFT;

    bool running = false;
    bool paused = false;
    bool compare_mode = false;
    float sim_speed = 1.0f;
};

} // namespace grip
