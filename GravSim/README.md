# GravitySimulator (GravSim)

A real-time 3D gravity simulation and visualization tool built with C++ and OpenGL.

## Project Description

GravSim is an interactive physics simulation that models gravitational interactions between multiple bodies in 3D space. It provides visualization of orbital mechanics, collision detection, and trajectory tracking with a user-friendly ImGui interface.

### Features

- **Real-time 3D Visualization**: OpenGL-based rendering with smooth camera controls
- **Physics Simulation**: Accurate gravitational force calculations using Newton's law of universal gravitation
- **Interactive UI**: ImGui interface for simulation control and parameter adjustment
- **Trail Tracking**: Visual trails showing the historical paths of celestial bodies
- **Customizable Parameters**:
  - Gravity constant scaling
  - Simulation speed control
  - Body mass, radius, and initial velocity
  - Collision detection modes

## Project Structure

```
GravSim/
├── CMakeLists.txt          # Build configuration
├── src/
│   └── Main.cpp            # Main application source code
├── external/               # External dependencies
│   ├── glfw/              # Window and input management
│   ├── glad/              # OpenGL loader
│   ├── glm/               # Math library (vectors, matrices)
│   └── imgui/             # UI framework
└── README.md              # This file
```

## Dependencies

- **GLFW**: Window creation and input handling
- **GLAD**: OpenGL function loader
- **GLM**: Graphics math library
- **ImGui**: Immediate mode GUI
- **OpenGL 3.3+**: Graphics API
- **C++17**: Language standard

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- OpenGL 3.3+ support
- Linux/macOS: Install development headers
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libgl1-mesa-dev xorg-dev
  
  # macOS
  brew install glfw3 glew
  ```

### Build Instructions

```bash
# Create and enter build directory
mkdir build
cd build

# Generate build files and compile
cmake ..
make

# Run the simulation
./GravSim
```

### Windows Build

Use CMake GUI or command line:
```bash
cmake --build .
```

## Usage

1. **Launch**: Run `./GravSim` from the build directory
2. **Add Bodies**: Use the ImGui interface to add gravitational bodies with:
   - Mass and radius
   - Initial position and velocity
   - Color assignment
3. **Simulate**: Start/pause the simulation using the UI controls
4. **Observe**: Watch orbital mechanics unfold with real-time visualization
5. **Adjust**: Modify simulation parameters in real-time

### Camera Controls

- **Mouse Look**: Right-click and drag to rotate view
- **WASD**: Move camera forward/backward/left/right
- **Space/Ctrl**: Move camera up/down
- **Scroll**: Adjust camera speed

## Technical Details

- **Physics Engine**: N-body gravitational simulation with softening factor for numerical stability
- **Rendering**: Sphere meshes with lighting and trail rendering
- **Timestep**: Configurable simulation timestep with gravity constant scaling for visualization
- **Collision Handling**: Multiple collision modes (bounce, merge, absorb)

## Author

Developed as part of COMP342 Computer Graphics course by
- **Sanjib Dahak (18)**
- **Aryaman Giri (21)**

