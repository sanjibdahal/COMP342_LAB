# COMP342 Computer Graphics Laboratory

A comprehensive computer graphics laboratory course implementing fundamental graphics algorithms and 3D transformations using Python, OpenGL, and NumPy.

## Project Overview

This repository contains 5 laboratory assignments covering:

- **LAB1**: Basic 2D Drawing and Rendering
- **LAB2**: Line Drawing Algorithms (DDA, Midpoint Circle, Graphs, Pie Charts)
- **LAB3**: 2D and 3D Transformations (Translation, Rotation, Scaling, Shearing)
- **LAB4**: Polygon Clipping (Cohen-Sutherland, Liang-Barsky, Sutherland-Hodgmann)
- **LAB5**: 3D Transformations and Projections (Orthographic and Perspective)
- **PROJECT**: Advanced 3D Rendering with GLFW and GLM

## Prerequisites

- **Python**: 3.11 or higher
- **UV**: Python package manager ([Install UV](https://docs.astral.sh/uv/))
- **OpenGL-capable GPU**: For rendering demonstrations
- **Operating System**: Windows, macOS, or Linux

## Installation and Setup

### 1. Install UV (Package Manager)

UV is a modern, fast Python package manager. Install it using one of these methods:

**Windows (using pip):**
```bash
pip install uv
```

**macOS/Linux (using curl):**
```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

**Or download from**: https://github.com/astral-sh/uv/releases

### 2. Clone the Repository

```bash
git clone https://github.com/sanjibdahal/COMP342_LAB.git
cd COMP342_LAB
```

### 3. Create Virtual Environment with UV

```bash
# Create a virtual environment
uv venv

# Activate the virtual environment
# On Windows:
.venv\Scripts\activate

# On macOS/Linux:
source .venv/bin/activate
```

### 4. Install Dependencies

```bash
# Using UV (recommended)
uv pip install -r requirements.txt

# OR using pip directly
pip install -r requirements.txt
```

**Required Packages:**
- `PyOpenGL==3.1.10` - Python bindings for OpenGL
- `glfw==2.10.0` - Python bindings for GLFW (window management)
- `numpy==2.4.0` - Numerical computing library
- `pygame` - For additional graphics and event handling

## Running the Labs

### LAB1: Basic 2D Drawing

Contains introductory OpenGL programs for drawing basic shapes.

```bash
cd LAB1

# Draw name/shapes with OpenGL
python drawing_name.py

# Display resolution and window information
python resolution.py
```

**What you'll see:**
- Basic 2D geometric shapes rendered with OpenGL
- Drawing primitives (lines, rectangles, polygons)

---

### LAB2: Line Drawing Algorithms & Graphics

Implements various line drawing algorithms and creates visual graphs.

```bash
cd LAB2

# Draw lines using Bresenham's Line Algorithm
python BLA.py

# Draw lines using DDA (Digital Differential Analyzer) Algorithm
python DDA.py

# Create a line graph with DDA algorithm
python LineGraph.py

# Draw circles using Midpoint Circle Algorithm
python Midpoint_circle.py

# Create a pie chart visualization
python Piechart.py
```

**Features:**
- Multiple line drawing algorithms
- Real-time graph visualization
- Chart generation

---

### LAB3: 2D and 3D Transformations

Demonstrates geometric transformations including translation, rotation, scaling, and shearing.

```bash
cd LAB3

# 2D Transformations (Translation, Rotation, Scaling, Shearing)
python 2D_Transformation.py

# Composite/Combined Transformations
python Composite_Transformation.py

# Ellipse rendering with transformations
python Ellipse.py
```

**Keyboard Controls:**
- `1`: Apply translation
- `2`: Apply rotation
- `3`: Apply shearing
- `4`: Apply scaling
- `R`: Reset transformation
- `ESC`: Exit program

---

### LAB4: Polygon Clipping Algorithms

Implements three major polygon clipping algorithms for removing portions of polygons outside a clipping window.

```bash
cd LAB4

# Cohen-Sutherland Line Clipping Algorithm
python cohen_sutherland.py

# Liang-Barsky Line Clipping Algorithm
python liang_barsky.py

# Sutherland-Hodgmann Polygon Clipping Algorithm
python sutherland_hodgemann.py
```

**Features:**
- Interactive clipping window visualization
- Multiple clipping algorithm implementations
- Real-time visual feedback

---

### LAB5: 3D Transformations and Projections

Advanced 3D graphics with transformations, projections, and multiple camera angles.

```bash
cd LAB5

# 3D Transformations (Pyramid with interactive transformations)
python 3d_transformation.py

# Orthographic Projection demonstration
python orthographic_projection.py
```

**Keyboard Controls for 3D Transformation:**
- `1`: Translation (4, 4, 0)
- `2`: Rotation about Y-axis (30°)
- `3`: Shearing along X-axis
- `4`: Uniform scaling (2x)
- `C`: Cycle through 8 camera positions
- `R`: Reset transformation
- `ESC`: Exit program

**Camera Positions:**
The program includes 8 different camera angles:
1. Front view
2. Right view
3. Back view
4. Left view
5. Top-right-front
6. Top-left-front
7. Top-right-back
8. Top-left-back

---

### PROJECT: Advanced 3D Rendering

Combines GLFW window management, GLM mathematics library, and OpenGL for advanced 3D graphics.

```bash
# Navigate to PROJECT directory
cd PROJECT

# Compile and run the C++ project
# See PROJECT folder for specific build instructions
```

**Features:**
- GLFW-based window management
- GLM mathematics library integration
- Advanced 3D rendering techniques

---

## Project Structure

```
COMP342_LAB/
├── LAB1/                    # Basic 2D drawing
│   ├── drawing_name.py
│   └── resolution.py
├── LAB2/                    # Line algorithms & graphics
│   ├── BLA.py
│   ├── DDA.py
│   ├── LineGraph.py
│   ├── Midpoint_circle.py
│   └── Piechart.py
├── LAB3/                    # 2D & 3D transformations
│   ├── 2D_Transformation.py
│   ├── Composite_Transformation.py
│   └── Ellipse.py
├── LAB4/                    # Polygon clipping
│   ├── cohen_sutherland.py
│   ├── liang_barsky.py
│   └── sutherland_hodgemann.py
├── LAB5/                    # 3D transformations
│   ├── 3d_transformation.py
│   └── orthographic_projection.py
├── PROJECT/                 # Advanced 3D rendering
│   ├── main.cpp
│   ├── second.cpp
│   ├── glad.c
│   ├── include/
│   └── lib/
├── glm-1.0.2/              # GLM mathematics library
├── pyproject.toml          # Python project configuration
├── requirements.txt        # Python dependencies
└── README.md              # This file
```

## Troubleshooting

### Import Errors

If you encounter `ModuleNotFoundError`:

```bash
# Ensure virtual environment is activated
# Then reinstall dependencies
uv pip install -r requirements.txt --force-reinstall
```

### OpenGL Errors

Common OpenGL issues:

1. **"No OpenGL context found"**
   - Ensure your GPU supports OpenGL
   - Update graphics drivers

2. **"GLFW initialization failed"**
   - On Linux: Install: `sudo apt-get install libglfw3-dev`
   - On macOS: Install via Homebrew: `brew install glfw3`

### GLFW Window Issues

If GLFW programs fail to open a window:

```bash
# Try reinstalling GLFW
uv pip uninstall glfw
uv pip install glfw==2.10.0
```

## Virtual Environment Cheat Sheet

```bash
# Create virtual environment
uv venv

# Activate (Windows)
.venv\Scripts\activate

# Activate (macOS/Linux)
source .venv/bin/activate

# Deactivate
deactivate

# Install packages
uv pip install -r requirements.txt

# List installed packages
uv pip list

# Freeze current environment
uv pip freeze > requirements.txt

# Remove virtual environment
rm -rf .venv  # macOS/Linux
rmdir /s .venv  # Windows
```

## Learning Resources

### Computer Graphics Concepts

- **2D Graphics**: Points, lines, polygons, circles
- **3D Graphics**: Transformations, projections, viewing
- **Algorithms**: Line drawing (DDA, Bresenham), clipping (Cohen-Sutherland, Liang-Barsky)
- **Projections**: Orthographic and perspective projections
- **Camera Systems**: Multiple viewpoints and camera management

### Recommended Reading

- **Computer Graphics: Principles and Practice** - Foley, van Dam, Feiner, Hughes
- **OpenGL Programming Guide** - Khronos Group
- **Real-Time Rendering** - Möller, Haines, Hoffman

## Common Errors and Solutions

| Error | Solution |
|-------|----------|
| `ModuleNotFoundError: No module named 'OpenGL'` | Run `uv pip install -r requirements.txt` |
| `GLFW initialization failed` | Update graphics drivers or install system dependencies |
| `Invalid window handle` | Close any existing windows before running new programs |
| `GL_INVALID_VALUE` | Ensure window is properly initialized before OpenGL calls |

## Contributing

This is a course assignment repository. Modifications and extensions are welcome for educational purposes.

## License

This project is created for educational purposes as part of COMP342 Computer Graphics course.

## Contact & Support

For issues or questions regarding the labs:
1. Check the error message in the console output
2. Review the troubleshooting section above
3. Verify all dependencies are installed: `uv pip list`
4. Ensure virtual environment is activated

---

**Last Updated**: January 2026
**Python Version**: 3.11+
**Package Manager**: UV
