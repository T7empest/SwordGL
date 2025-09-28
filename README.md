# SDL3 GPU Particle Renderer

A tiny playground for **instanced particles** on **SDL3 GPU** with **soft AA edges** (screen-space `fwidth`), **organic shapes** (ellipse + domain warp + noise), and a short **orange “ember”** burst masked to the particle shape. Background is a minimal shader (noise/raymarch vibe).  
**Study project:** kept small so you can tweak shaders fast.

---
<img width="793" height="794" alt="image" src="https://github.com/user-attachments/assets/74272a20-5fb7-4e58-bb13-a0d98808761c" />

## Features
- Instanced rendering: one quad + per-instance data (`world_pos`, `size` in px, `color`, `age01`).
- **Soft, blurred edges** (anti-aliased in screen space).
- **Non-circular silhouettes**: random ellipse, warped outline, subtle grain.
- **Ember glow** at birth/end, masked to the particle interior.
- Unified **std140 UBO** for fragment shaders: `vec2 resolution; float time;`.
- **Straight-alpha blending** (`SRC_ALPHA / ONE_MINUS_SRC_ALPHA`), depth test off for particles.
- Optional **ImGui** overlay for quick debugging.

---

## Tech Stack
- **SDL3 GPU** (+ ShaderCross for runtime SPIR-V)
- **GLM** (math)
- **Dear ImGui** (debug UI)
- CMake + Ninja / MSVC / Clang (cross-platform)

---

## Quick Start
```bash
git clone --recursive <this-repo>
cmake -S . -B build -G Ninja
cmake --build build
# run the app from build/bin (or your generator’s output dir)
