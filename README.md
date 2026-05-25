# Vacuum Jamming Gripper — Fuzzy Control Simulation

C++ simulation of a particle jamming gripper with fuzzy-logic pressure control.
Built with Dear ImGui, ImPlot, SDL2, and OpenGL 3.3.

## Dependencies

- SDL2
- CMake ≥ 3.16
- C++17 compiler
- OpenGL (libGL)

```bash
sudo apt install libsdl2-dev cmake g++
```

## Build

```bash
# 1. Create third_party directory
mkdir -p third_party

# 2. Download Dear ImGui v1.91.8
wget -O /tmp/imgui.zip https://github.com/ocornut/imgui/archive/refs/tags/v1.91.8.zip
unzip /tmp/imgui.zip -d third_party/
mv third_party/imgui-1.91.8 third_party/imgui

# 3. Download ImPlot v0.16
wget -O /tmp/implot.zip https://github.com/epezent/implot/archive/refs/tags/v0.16.zip
unzip /tmp/implot.zip -d third_party/
mv third_party/implot-0.16 third_party/implot

# 4. Build
cmake -B build && cmake --build build

# 5. Run
./build/vacuum_gripper
```

## Project structure

```
src/
├── main.cpp                     Entry point
├── core/types.h                 Shared data structures
├── simulation/
│   ├── physics.h/.cpp           Pressure dynamics + volume elasticity
│   ├── motor.h/.cpp             DC motor model (PWM → V/I/P/flow)
│   ├── gripper.h/.cpp           Jamming rigidity curve
│   └── fuzzy_controller.h/.cpp  Mamdani fuzzy logic engine
└── ui/
    ├── app.h/.cpp               Main loop, window, orchestration
    ├── dashboard.h/.cpp         Control panel (sliders, readouts)
    ├── plots.h/.cpp             Real-time ImPlot charts
    └── visuals.h/.cpp           2D gripper rendering
```
