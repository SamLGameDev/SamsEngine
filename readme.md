
##Future plans

I am going to be implementing a 3D Voronoi algorithm using Vulkan. Using Vulkan for this is important, as there will be a lot of different points to calculate, and being able to take advantage of the parallelism of Vulkan compared to OpenGL will massively improve performance.

## How im going to do it

I’m going to be implementing it by adapting my current 2D algorithm to work in 3D. As a high-level overview, I will first generate some points in the bounding box of the mesh. Then, for each point, I will be creating a plane perpendicular bisector at the halfway point between the point closest to my current point, and the current point. Then, for each face of the bounding box, find the first intersection point, then record each vertex until you find the second intersection. Then check if the point is within this shape, if not, reverse it. then add this new face to a new array of new cell. Once this is done for all faces, we have the first cut. Then Check if any points are still within the bounds, if so, repeat for the next nearest point in bounds. After this, we have a fracture and just repeat the whole thing for every point.

##Optimisation 

Currently, there are no major bottlenecks for my program, but here are some issues that will help my next 3D iteration run faster.

![image](https://github.falmouth.ac.uk/user-attachments/assets/347632b3-4130-4008-ba24-58fd7d0afbf3)

Here, It shows that creating my point shader is very inefficient, so for my next iteraction ill Move this out of fracture peice creation, so its only ever created once

![image](https://github.falmouth.ac.uk/user-attachments/assets/3f76fc17-909c-440b-929f-0a0f5b9fce8f)

Assinging to the heap for each fracrue peice is very inefficent, so ill rewrite the code so it can be stack based


![image](https://github.falmouth.ac.uk/user-attachments/assets/e75faf0a-7292-465f-a9bd-3c1b207ce0ac)

Copying the newcell to cell is inefficnet, so i can rewrite the program to use newcell instead, avoiding the copying

![image](https://github.falmouth.ac.uk/user-attachments/assets/f1104a81-8001-4c11-8860-a3f5e65248ea)

Adding to my array is expensive, so ill rewrite the add method to allocate space to double the current size when running out, avoiding so many new allocations

##References

Voronoi 2D explanation https://www.youtube.com/watch?v=I6Fen2Ac-1U

Efficent vornoi https://www.researchgate.net/publication/221209166_Efficient_Computation_of_3D_Clipped_Voronoi_Diagram

Spatial Tellestations https://onlinelibrary.wiley.com/doi/book/10.1002/9780470317013

Efficient computation of clipped Voronoi diagram for mesh generation https://www.sciencedirect.com/science/article/abs/pii/S0010448511002351

## Building this project
This project uses the cmake build system, this is a cross-platform build system with wide support outside of the Microsoft ecosystem. Many of the FOSS projects, such as OBS Studio and SDL are developed with this build system. You can view these on their github pages.

### Windows
On windows, the path of least resistance is to install visual studio community edition, along with the C++ tools (and cmake support). You can then open the cmake project from the file menu (I don't know of a way to open an existing cmake project from the launcher - sorry). 

There is a step by step guide I wrote for security. You can find it in the [COMP280 Resources](http://fal.fosslab.uk/comp280/unreal/sec_pt1/). Ignore the bit about the adventure game in the guide.

### Linux/CLI
If you have a cmake CLI, toolchain and compiler setup. You can use modern cmake commands to build this project:

```bash
cmake -B build/ -S .
cmake --build build
```

Another option is to use CLion, you can get free Educational licences while you are a student. If you've used pycharm, it's pycharm for C++.
