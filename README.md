# elora

C++14 CPU 3D engine — draws to a back buffer, then presents to an X11 window.

## Highlights

- Start and shut down `elora::Engine`
- Application supplies its name and window size (the bundled demo uses 3D Demo at 1024x768)
- Window title includes the resolution, e.g. `3D Demo 1024x768`
- Draw to a CPU back buffer (`clear`, `put_pixel`, `fill_triangle`, `stroke_triangle`, `draw_text`); `present()` copies the buffer 1:1 to the X11 window
- `Engine::delta_time()` and `Engine::fps()` come from the wall clock between frames (no sleep or FPS cap); camera motion and mesh rotation scale by `dt`
- `elora::update` composes a frame in the back buffer (depth-buffered mesh, Lambert lighting, and HUD); `present()` is the only copy to the X11 window
- `elora::Vector3D` for positions and directions, with rotations about X, Y, and Z
- `elora::Camera` look-at perspective projection (position, target, FOV)
- `elora::Triangle` (indexed) and `elora::Mesh` (vertices plus triangles)
- Load and save Wavefront `.obj` meshes (`assets/cube.obj`, plane, pyramid, tetrahedron, octahedron, teapot)
- Bundled demo loads `assets/teapot.obj` and spins it in one color; move the camera with WASD (Z/X), R/F (Y), or arrow keys; **T** toggles wireframe; **C** toggles a per-face palette; **P** toggles auto-rotation; an FPS readout sits on the back buffer
- Build with g++ (C++14) through a Makefile workflow
- Catch2 tests for the engine lifecycle and drawing

## Prerequisites

- **g++ with C++14** — required to build from source
- **libx11** — required to open the window (`libx11-dev` on Debian/Ubuntu)
- **cppcheck** — required for `make lint`
- **clang-format** — required for `make fmt`

## Installation

### Build from Source

```bash
git clone https://github.com/carlosrabelo/elora.git
cd elora
make setup
make build
```


## Usage

```bash
./bin/elora
```

Opens a window titled `3D Demo 1024x768` and spins the teapot loaded from `assets/teapot.obj`. Move the camera: **A/D** or arrows X, **R/F** Y, **W/S** or arrows Z. **T** toggles wireframe. **C** toggles per-face colors (default is one color). **P** pauses or resumes auto-rotation. **Space** resets the camera and rotation. Close with Escape, `q`, or the window manager.

## Project Layout

```
elora/               # C++ component (sources, tests, Makefile)
  src/core/          # Optional, Span
  src/engine/        # Engine facade, keys, canvas, X11 window
  src/math/          # Vector3D, Camera
  src/mesh/          # Triangle, Mesh, primitives
  src/render/        # pipeline and mesh rasterizer
  src/io/            # Wavefront OBJ
assets/              # Wavefront .obj meshes
.make/               # Build and test scripts
bin/                 # Compiled binaries (git-ignored)
```

## Development

```bash
make setup           # Download Catch2 header (first time only)
make build           # Compile binary to bin/elora
make test            # Build and run tests
make quality         # Format, lint, and test
```

## License

This project is licensed under the GNU General Public License v3.0 — see [LICENSE](LICENSE) for details.
