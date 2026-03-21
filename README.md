# WebGL Water — OpenGL/GLUT Port

A real-time water simulation with wave propagation, caustics, reflections,
refractions, and a movable sphere — ported from Evan Wallace's WebGL Water
demo to desktop OpenGL 3.3 + freeglut + GLEW.

---

## Features

- GPU heightfield wave simulation (ping-pong FBOs, GLSL 3.30)
- Caustics projection using refracted light rays + dFdx/dFdy
- Fresnel reflection/refraction on the water surface
- Skybox environment mapping (procedural gradient sky)
- Analytic ambient occlusion on pool walls
- Movable reflective sphere
- Toggleable gravity / sloshing

---

## Prerequisites

| Tool | Where to get it |
|------|----------------|
| MinGW-w64 (g++ 12+) | https://winlibs.com  — download UCRT, POSIX, x86_64 |
| VSCode | https://code.visualstudio.com |
| VSCode C/C++ extension | ms-vscode.cpptools (install from Extensions panel) |

---

## Step 1 — Install MinGW-w64

1. Go to **https://winlibs.com**
2. Download the latest **Win64 UCRT — POSIX** `.zip` (not the installer)
3. Extract to `C:\mingw64`  (so `C:\mingw64\bin\g++.exe` exists)
4. Add `C:\mingw64\bin` to your **System PATH**:
   - Windows search → "Environment Variables"
   - Under System Variables → Path → Edit → New → `C:\mingw64\bin`
5. Open a new terminal and verify:
   ```
   g++ --version
   ```

---

## Step 2 — Get freeglut (prebuilt for MinGW)

1. Go to **https://www.transmissionzero.co.uk/software/freeglut-devel/**
   (the most reliable prebuilt MinGW freeglut)
2. Download `freeglut-MinGW-3.0.0-1.mp.zip` (or latest)
3. Extract. Inside you will find:
   ```
   freeglut/
     include/GL/freeglut.h  freeglut_std.h  freeglut_ext.h  glut.h
     lib/x64/libfreeglut.a  libfreeglut_static.a
     bin/x64/freeglut.dll
   ```
4. Copy files into this project:
   - `include/GL/*.h`    → `water-sim/include/GL/`
   - `lib/x64/*.a`       → `water-sim/lib/`   (rename to `libfreeglut.a`)
   - `bin/x64/freeglut.dll` → `water-sim/bin/`

---

## Step 3 — Get GLEW (prebuilt)

1. Go to **https://glew.sourceforge.net/**
2. Download **Binaries — Windows 32-bit and 64-bit**
3. Extract. Inside:
   ```
   glew/
     include/GL/glew.h  wglew.h
     lib/Release/x64/glew32.lib   glew32s.lib
     bin/Release/x64/glew32.dll
   ```
4. Copy into project:
   - `include/GL/glew.h` `wglew.h` → `water-sim/include/GL/`
   - `lib/Release/x64/glew32.lib`  → `water-sim/lib/`
     (MinGW needs it named `libglew32.a` — see note below)
   - `bin/Release/x64/glew32.dll`  → `water-sim/bin/`

   > **MinGW .lib → .a note:**  MinGW cannot link `.lib` files directly.
   > Run this once in the `lib/` folder:
   > ```
   > gendef ../bin/glew32.dll
   > dlltool -d glew32.def -l libglew32.a
   > ```
   > This creates `libglew32.a` that g++ can link with `-lglew32`.

---

## Step 4 — Get GLM (header-only, no build needed)

1. Go to **https://github.com/g-truc/glm/releases**
2. Download the latest source zip
3. Extract the `glm/` folder (the one containing `glm.hpp`) into:
   ```
   water-sim/include/glm/
   ```
   So you have `water-sim/include/glm/glm.hpp` etc.

---

## Step 5 — Final folder structure

After all steps your project should look like:

```
water-sim/
├── .vscode/
│   ├── tasks.json
│   ├── launch.json
│   └── c_cpp_properties.json
├── include/
│   ├── GL/
│   │   ├── glew.h
│   │   ├── wglew.h
│   │   ├── freeglut.h
│   │   ├── freeglut_std.h
│   │   ├── freeglut_ext.h
│   │   └── glut.h
│   └── glm/
│       ├── glm.hpp
│       ├── gtc/
│       └── ... (rest of GLM)
├── lib/
│   ├── libfreeglut.a
│   └── libglew32.a
├── bin/
│   ├── freeglut.dll      ← must be here at runtime
│   └── glew32.dll        ← must be here at runtime
├── shaders/
│   ├── quad.vert
│   ├── water_update.frag
│   ├── water_drop.frag
│   ├── water_normal.frag
│   ├── caustics.vert
│   ├── caustics.frag
│   ├── pool.vert
│   ├── pool.frag
│   ├── water.vert
│   ├── water.frag
│   ├── sphere.vert
│   ├── sphere.frag
│   ├── sky.vert
│   └── sky.frag
└── src/
    ├── common.h
    ├── main.cpp
    ├── water.h
    ├── water.cpp
    ├── renderer.h
    └── renderer.cpp
```

---

## Step 6 — Build

Open the project folder in VSCode:
```
File → Open Folder → water-sim/
```

Build with **Ctrl+Shift+B** (runs the default build task).

Or from a terminal in the project root:
```bash
g++ -std=c++17 -O2 -Wall ^
    src/main.cpp src/water.cpp src/renderer.cpp ^
    -I./include -I./include/glm ^
    -L./lib ^
    -lfreeglut -lglew32 -lopengl32 -lglu32 -lgdi32 ^
    -o bin/water.exe
```

Then run:
```bash
cd bin
water.exe
```

> **Important:** Run from the project root (not from inside `bin/`), OR copy
> the `shaders/` folder next to `water.exe`. The executable looks for shaders
> relative to its working directory.
> The easiest approach is to always run from the project root:
> ```bash
> cd water-sim
> bin\water.exe
> ```

---

## Controls

| Input | Action |
|-------|--------|
| Left-click + drag on water | Create ripples |
| Left-click + drag on sphere | Move sphere |
| Right-click + drag | Rotate camera |
| Scroll wheel | Zoom in/out |
| `SPACE` | Pause / resume simulation |
| `G` | Toggle gravity / sloshing |
| `L` + left-drag | Reposition sun direction |
| `ESC` | Quit |

---

## Troubleshooting

**Black window / no output:**
- Make sure `glew32.dll` and `freeglut.dll` are in `bin/` next to `water.exe`
- Run from the project root so shader paths resolve correctly
- Check the console output for shader compile errors

**`glew.h: No such file`:**
- Verify `include/GL/glew.h` exists
- Check the `-I./include` flag in the build command

**`cannot find -lglew32`:**
- You need `lib/libglew32.a` (see the `gendef`/`dlltool` step above)

**Shader errors at runtime:**
- The console prints GLSL compile logs — read them carefully
- They tell you the exact line and variable that failed

**Very slow / low FPS:**
- Make sure you compiled with `-O2`
- Ensure your GPU drivers are up to date
- The water grid is 128×128 by default — reduce to 64 in `renderer.cpp`
  (`buildWaterMesh(64, ...)`) if needed

---

## How it works (quick reference)

```
Each frame:
  1. water.update()
       ├── wave propagation pass  (GPU FBO ping-pong, finite difference)
       └── normal map pass        (central difference on heightfield)

  2. renderer.render()
       ├── caustics pass          (project refracted light rays to floor)
       ├── skybox                 (environment cube map)
       ├── pool pass              (tiles + caustics + AO + soft shadow)
       ├── sphere pass            (Phong + env reflection + Fresnel)
       └── water surface pass     (displacement + Fresnel + reflection/refraction)
```
