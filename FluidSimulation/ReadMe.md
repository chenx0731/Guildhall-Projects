[Fluid Simulation - Chen Xu](https://www.chenoa.site/personal-project/fluid-simulation)

---

| ROLE      | Graphics&physics programmer                |

| Engine    | Custom Game Engine                         |

| Dev Time  | Aug 2023 - Present                         |

| Languages | C++, HLSL                                  |

| Tools     | Visual Studio, DirectX11, RenderDoc, ImGui |

Fluid simulation is a challenging problem in terms of physics simulation and realistic rendering. This project focuses on both the simulation and rendering to make the fluid interactive and look satisfying.

For this project, I choose Position Based Fluid as the simulation solver and Screen Space Rendering for real time rendering. And I develop this project on my C++ custom game engine using DirectX11 and DX compute shader for parallelization computing.



## Current Features

- Position based fluid
  - The solver for physics simulation, it's a lagrangian-based system utilizing a Jacobi-style update for compute all particles in parallel on the GPU.

- Screen space rendering
  - Generate depth, thickness image of all particles and get the normal map in view space using depth image, and render the final image by using Blinn-Phong reflection model and Beer's law for refraction.

- Spatial hashing neighbor searching
  - A neighbor searching method for inserting all particles into cells based on the SPH kernel radius, each particle can easily get its neighbors in 27 cells of its local area to compute its density and velocity changed by viscosity.

- Smoothed depth / thickness image
  - Get smoothed depth/thickness image by using bilateral filter to keep the boundary, which makes the surface of water look less particle-like.

- Skybox reflection with Fresnel
  - Sample the sky cube texture based on reflection ray and adjust reflection and refraction based on Fresnel scaler.
 
- Caustics
  - Use image-space technique, find the intersections of refracted rays and ground, and render the points with additive blending.

## Figures

- Rendering Pipeline Graph
![Rendering Pipeline](https://github.com/chenx0731/Fluid-Simulation/blob/main/Run/Captures/Rendering%20Pipeline.png?raw=true)

- Skybox Reflection Gif
![Fluid_CubemapR](https://github.com/chenx0731/Fluid-Simulation/blob/main/Run/Captures/Fluid_CubemapR.gif?raw=true)

- Caustics Gif
![Fluid_Caustics](https://github.com/chenx0731/Fluid-Simulation/blob/main/Run/Captures/Fluid_Demo_Caustics.gif?raw=true)
