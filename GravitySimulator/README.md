# Gravity Simulator - N-Body Physics Visualization

A comprehensive 3D gravity simulation and visualization system implementing real Newtonian physics with interactive controls and multiple preset scenarios.

## 📋 Project Overview

This project demonstrates:
- **Real Physics**: Newton's Law of Universal Gravitation (F = G × m₁ × m₂ / r²)
- **Advanced Integration**: RK4 (Runge-Kutta 4th order) for numerical stability
- **Collision Physics**: Elastic collision detection and resolution
- **Multiple Scenarios**: Pre-configured simulations including Three-Body Problem
- **Interactive 3D Visualization**: OpenGL-based rendering with realistic lighting
- **Educational Tools**: Force/velocity vectors, orbital trails, energy conservation

## 🎮 Simulation Modes

### 1. Solar System
A simplified model of our solar system with the Sun and inner planets demonstrating stable orbital mechanics.

### 2. Three-Body Problem
The classic chaotic three-body system showing sensitive dependence on initial conditions. This is one of the most famous problems in physics and demonstrates how simple systems can exhibit complex, unpredictable behavior.

### 3. Binary Star System
Two massive stars orbiting their common center of mass with circumbinary planets.

### 4. Figure-8 Orbit
A stable periodic solution to the three-body problem discovered by Chenciner and Montgomery in 2000.

### 5. Galaxy Collision
Simulation of two small galaxies colliding, demonstrating gravitational interactions on a larger scale.

### 6. Sandbox Mode
Empty canvas where you can interactively add and remove bodies to create custom scenarios.

## 🎯 Features

### Physics Engine
- **Gravitational Forces**: Accurate N-body gravitational calculations
- **Collision Detection**: Sphere-sphere collision with elastic response
- **Energy Conservation**: System tracks and displays total energy
- **Adaptive Integration**: Both Euler and RK4 methods available
- **Softening Parameter**: Prevents numerical singularities

### Visualization
- **3D Spheres**: Procedurally generated with proper lighting and shading
- **Orbital Trails**: Historical paths showing body trajectories
- **Vector Display**: Real-time force and velocity vectors
- **Reference Grid**: Coordinate grid and axes for spatial reference
- **Dynamic Camera**: Full 3D camera control with orbit, pan, and zoom

### Interaction
- **Time Control**: Pause, resume, and adjust simulation speed
- **Body Management**: Add, remove, and select bodies
- **Real-time Editing**: Modify simulation while running
- **Multiple Views**: Switch between different camera modes

## 🔧 Technical Implementation

### Physics Calculations

**Newton's Law of Gravitation:**
```
F = G × (m₁ × m₂) / r²
```

**Softening to Prevent Singularities:**
```
r_eff = max(r, ε)
```
Where ε is the softening parameter.

**RK4 Integration:**
Uses 4th-order Runge-Kutta for accurate trajectory calculation:
```
k₁ = f(t, y)
k₂ = f(t + dt/2, y + k₁×dt/2)
k₃ = f(t + dt/2, y + k₂×dt/2)
k₄ = f(t + dt, y + k₃×dt)
y_new = y + (k₁ + 2k₂ + 2k₃ + k₄) × dt/6
```

### Rendering Pipeline

1. **Geometry Generation**: Procedural sphere mesh using parametric equations
2. **Vertex Shader**: Transforms vertices to world/view/projection space
3. **Fragment Shader**: Phong lighting model with ambient, diffuse, and specular components
4. **Post-Processing**: Line rendering for trails and vectors

## 📦 Installation

### Dependencies

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libglfw3-dev libglew-dev libglm-dev
```

#### macOS
```bash
brew install cmake
brew install glfw glew glm
```

#### Windows
1. Install [Visual Studio 2019+](https://visualstudio.microsoft.com/)
2. Download pre-built libraries:
   - [GLFW](https://www.glfw.org/download.html)
   - [GLEW](http://glew.sourceforge.net/)
   - [GLM](https://github.com/g-truc/glm/releases)

### Building

#### Using CMake (All Platforms)
```bash
# Clone or download the project
cd GravitySimulator

# Create build directory
mkdir build && cd build

# Generate build files
cmake ..

# Compile
cmake --build .

# Run
./GravitySimulator
```

#### Manual Compilation (Linux/macOS)
```bash
g++ -std=c++17 src/*.cpp -o GravitySimulator \
    -lGL -lGLEW -lglfw -lm \
    -I./libs -I/usr/include/glm
```

## 🎮 Controls

### Keyboard
| Key | Action |
|-----|--------|
| `1-6` | Switch simulation modes |
| `SPACE` | Pause/Resume simulation |
| `R` | Reset current simulation |
| `T` | Toggle orbital trails |
| `V` | Toggle velocity vectors |
| `F` | Toggle force vectors |
| `G` | Toggle reference grid |
| `A` | Toggle coordinate axes |
| `+/-` | Increase/Decrease time speed |
| `N` | Add new random body |
| `DELETE` | Remove selected body |
| `C` | Clear all trails |
| `ESC` | Exit application |

### Mouse
| Input | Action |
|-------|--------|
| `Left Click + Drag` | Rotate camera |
| `Right Click + Drag` | Pan camera |
| `Scroll Wheel` | Zoom in/out |
| `Click on Body` | Select body (shows info) |

## 📊 Educational Value

### Physics Concepts Demonstrated

1. **Universal Gravitation**: Every mass attracts every other mass
2. **Inverse Square Law**: Force decreases with square of distance
3. **Conservation Laws**: Total energy and momentum are conserved
4. **Orbital Mechanics**: Stable and unstable orbits
5. **Chaos Theory**: Small changes lead to drastically different outcomes (Three-Body)
6. **Numerical Methods**: Integration techniques for differential equations

### Visualizations

- **Force Vectors (Red)**: Show direction and magnitude of gravitational forces
- **Velocity Vectors (Green)**: Display current motion direction and speed
- **Orbital Trails**: Historical paths help visualize trajectory patterns
- **Energy Display**: Monitor total system energy to verify conservation

## 🔬 Three-Body Problem Explanation

The three-body problem is one of the oldest unsolved problems in classical mechanics. While two bodies can be solved analytically, three or more bodies generally cannot. This simulation demonstrates:

- **Chaos**: Tiny changes in initial conditions produce wildly different outcomes
- **Unpredictability**: Long-term behavior cannot be precisely predicted
- **Periodic Solutions**: Some special configurations (like Figure-8) are stable
- **Energy Transfer**: Complex exchanges of kinetic and potential energy

## 📈 Performance

- **Optimized for Real-time**: 60+ FPS with 20+ bodies
- **Scalable**: Handles up to 100 bodies on modern hardware
- **Efficient Rendering**: Instanced sphere rendering with shared geometry
- **Smart Updates**: Physics timestep independent of frame rate

## 🐛 Troubleshooting

### Black Screen
- Ensure graphics drivers are updated
- Check OpenGL version: `glxinfo | grep "OpenGL version"` (Linux)

### Slow Performance
- Reduce number of bodies
- Disable trails (`T` key)
- Lower time scale

### Compilation Errors
- Verify all dependencies are installed
- Check C++17 support: `g++ --version`
- Ensure library paths are correct in CMakeLists.txt

## 📝 Project Structure
```
GravitySimulator/
├── src/
│   ├── main.cpp              # Main application & simulation scenarios
│   ├── Body.{h,cpp}          # Celestial body class
│   ├── PhysicsEngine.{h,cpp} # Physics calculations
│   ├── Camera.{h,cpp}        # 3D camera system
│   ├── Shader.{h,cpp}        # Shader management
│   ├── Renderer.{h,cpp}      # OpenGL rendering
│   └── UI.{h,cpp}            # User interface
├── libs/
│   └── stb_image.h           # Image loading (optional)
├── CMakeLists.txt            # Build configuration
└── README.md                 # This file
```

## 🎓 For Students

This project covers:
- **Computer Graphics**: 3D rendering, shaders, lighting
- **Physics Simulation**: Numerical integration, collision detection
- **Software Engineering**: Multi-file structure, modularity
- **Mathematics**: Vector math, differential equations
- **User Interaction**: Event handling, camera controls

## 📚 References

- Newton, I. (1687). *Philosophiæ Naturalis Principia Mathematica*
- Chenciner, A., & Montgomery, R. (2000). "A remarkable periodic solution of the three-body problem"
- Hairer, E., Nørsett, S. P., & Wanner, G. (1993). *Solving Ordinary Differential Equations I*

## 📄 License

This project is created for educational purposes as a Computer Graphics course assignment.

## 👥 Author

Computer Graphics Course Project - Gravity Simulation

---

**Note**: This is a educational simulation. While physics is accurate within the classical mechanics framework, scale factors are adjusted for visualization purposes.