# 3D OpenGL Renderer

A simple 3D mountain top scene using OpenGL and C++.

# Features

- Physical based rendering with albedo, normal, metallic, roughness and ambient occlusion maps, with fallback values if a material doesn't include one.

#### Terrain

- Procedural generated terrain using Perlin noise offsetting the vertices of a plane.
- The terrain is textured using blending depending on the height. Lower terrain is textured with grass then it blends into rock, then at the top blends into white snow.

#### Transparency

- There are 2 transparent planes one red, one orange.
- The scene tree is traversed and all the transparent nodes are collected and are then sorted back to front and drawn after all opaque objects.
- Transparency values are provided to the Model constructor, which is then passed to the shader. The particle emitter uses transparent textures, the particles are also sorted back to front and drawn after opaque objects.

#### Particle Emitter

- A particle emitter was made to simulate a fire.
- It uses instancing which helps on performance, one quad is defined then instances of that single quad are dawn in one call. The particles are billboarded (always faces the camera).
- It uses a texture atlas for both the fire and smoke. The shader interpolates the colour from an orange to darker grey over the particles lifetime as well as fading out the particle. Particles are given random velocities and lifetimes in a range, then when they reach the end of their lifetime they get re-spawned.
- The particles are also affected by a wind value with the smoke particles being affected more than the fire.

#### Lights

- Directional light which moves in an arc overhead which also changes colour over time, simulating a day/might cycle.
- Spotlight which can be aimed using the `F` key.
- A warm orange point light is placed in the base of the campfire to provide an orange glow.

#### Shadows

- Shadows can be cast by both directional and spotlights.
- The directional light uses an orthographic projection matrix and the spotlight uses a perspective projection matrix.

#### Curves

- A ball follows a Bezier curve for its animation.
- It uses the current time elapsed to determine how far along the path it should be and which direction it should be going along the path

#### Camera

- FPS style camera.
- `WASD` to move
- `Space` to go up
- `CTRL` to go down
- `F` aims spotlight
- `Mouse` to look around

# Installation

This project using CMake as its build system.

Build instructions might differ depending on operating system.

### Linux

Navigate to the top directory then run:

```
cmake -S . -B build
cmake --build build
```

The project can then be run by:

```
cd build
./3DRender
```

# Model Attributes

- Picnic bench: <https://sketchfab.com/3d-models/picnic-bench-downloadable-50e979c0d26b4405a445a27253ada4f7>
- Cave walls: <https://www.turbosquid.com/3d-models/cave-walls-1-base-2076385>
- Arrow: <https://www.cgtrader.com/free-3d-models/various/various-models/cc0-arrow-5>
- Campfire: <https://sketchfab.com/3d-models/extinguished-campfire-free-download-b2108ef43be241e9b8ea716b47873bf5>
- Ball: <https://sketchfab.com/3d-models/rusty-metal-sphere-65e54b2d2e294075845b2ee73cd721aa>
