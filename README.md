# OpenGL Bedroom 2.0 (Shader-fied) - Final Project Submission
Tristan English\
Advanced Computer Graphics (CSCI 4239) Fall 2022

## Screenshot

<img width="595" alt="bedroom" src="https://user-images.githubusercontent.com/71680462/230977126-93a92f38-7d58-4bca-bea2-6badbbcfd371.png">

## Description

For this project, I reworked my original final project from CSCI 4229, adding new and improved textures with normal mapping. 
While I wasn't able to get TBN matrices working, it ended up looking fairly accurate, since much of the scene consists of simple quads and cuboids. 
For the more complex objects, like the luxo lamp, I was able to selectively render them without normal mapping to keep relatively accurate lighting across the scene.

Additionally, I experimented with more stylized shaders, modifying the toon shader from Lighthouse 3D. I added a few more shading sections, a white specular highlight, and sampled the color from the vertex shader to use the base color in the toon shader. 
What I'm most proud of here comes in post processing. Looking at cel shading online, I found it typically has a black outline - similar to a pen stroke in hand-drawn animation. 
To emulate this look, I used the frame buffer to calculate Sobel edge detection, inverted the result, and multiplied it by the pixel color to produce a black outline, which gave me the effect I was looking for! 
To increase the thickness, you can increase the number of post-processing passes, though this comes with some coloring artifacts from the edge detection.

Finally, I played around with some more post-processing effects, since I went to the effort of setting up the frame buffer. 
I have two black and white shaders (one using the averaging method and the other using the luminance method), a red, green, and blue filter, an inverse filter, a high contrast filter, and a color swap filter using a color matrix I create in the shader. 
These were super fun to play around with, and come with the added bonus of being able to apply them to the regular scene, the normal mapped scene, and the toon scene, since I'm stacking the shaders on top of each other.


## Run Instructions

To run, just run `make` and `./final`.

Key bindings\
  m/M &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Cycle through shaders\
  g/G &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Cycle through post-processing shaders\
  n/N &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Increase/decrease number of post-processing passes\
  r/R &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Cycle through scenes\
  t/T &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Cycle through textures (for solo quad render only)\
  s/S &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Start/stop light\
  p/P &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Toggle between orthogonal & perspective projection\
  -/+ &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Change light elevation\
  a &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Toggle axes\
  arrows &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Change view angle\
  Z/X &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Zoom in and out\
  0 &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Reset view angle\
  ESC &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Exit
  
## References
Texture images sourced from textures.com, CSCIx229 library sourced from Willem A. (Vlakkies) Schreuder.

