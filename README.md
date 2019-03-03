# Water Simulation

A water simulation project using OpenGL library
- The reflection and the refraction of the water are made using two FrameBuffer Objects, storing the renders in two textures that are later mapped on the water surface in the Fragment Shader
- The waves are simulated using Gerstner waves in the Geometry Shader

The Gerstner waves count can be adjusted using **Numpad 8** and **Numpad 2**, while the other parameters can be shuffled again using the **R** key.

## Known issues
When the waves count is bigger than 2 or 3, there are some glitches in the water(as seen in the GIF below, these "holes").

# Small demo
![GIF](https://thumbs.gfycat.com/LeadingHotBrocketdeer-size_restricted.gif)
