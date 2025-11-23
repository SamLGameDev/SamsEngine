
## Description

This project aims to implement a 3D voronoi fracturing algorithm [1] using vulkan[2]. Vulkan has been chosen over alternatives like OpenGL[3] due to its ability to take advantage of the parallelism of modern GPU's [4], which will be cruicial in this project as there will be many points to calculate which can be calculated independently of one another. A triangulation algorithm will also be needed to generate a mesh from the fractured points, and as such delaunay triangulation [8] will be used. This should result in significant performance gains. For future work, this project would aim at implimenting a clipped voronoi diagram[7], allowing the fracturing of a complex 3D mesh. The project has currently generated a voronoi diagram in 3D, though it still needs a triangulation algorithm added to function properly. This is what will be added in worksheet 4.

## Methodology
This project is going to be implimented by adapting the current half plane clipping algorithm for 3D [5][6]. The pesudocode for that can be found in fig 1.

```plaintext
Array<Vector3D> points = GenerateRandomPointsInBounds();
for (size_t i = 0; i < points.GetSize(); i++)
{
      Vector3D currentPoint = points[i];
      Array<Array<Vector3D>> cell = GetAllFacesOfBoundingBox();
      for (size_t j = 0; j < points.GetSize(); j++)
      {
            Array<Array<Vector3D>> newCell;
            if (i == j) continue;
            Vector3D comparedPoint = points[j];
            Vector3D perpendicularBisector = GetPerpendicularBisector(currentPoint, comparedPoint);
            for (Array<Vector3D> Face : cell)
            {
                  Array<Vector3D> newFace;
                  for (size_t p = 0; p < Face.GetSize(); p++)
                  {
                        Vector3D currentVert = Face[p];
                        Vector3D nextVert = Face[(p + 1) % Face.GetSize];
                        Vector3D intersectionPoint;
                        bool DoesPlaneIntersect = GetIntersectionPoint(perpendicularBisector, currentVert, nextVert, &intersectionPoint);
                        if (DoesPlaneIntersect)
                        {
                              newFace.Add(IntersectionPoint);
                              break;
                        }
                  }
                  if (newface.IsEmpty()) newFace = Face;
                  else{
                      for (size_t SecondIntersectionPoint = p; SecondIntersectionPoint < Face.GetSize(); SecondIntersectionPoint++)
                      {
                          Vector3D currentVert = Face[SecondIntersectionPoint];
                          Vector3D nextVert = Face[(SecondIntersectionPoint + 1) % Face.GetSize];
                          Vector3D intersectionPoint;
                          bool DoesPlaneIntersect = GetIntersectionPoint(perpendicularBisector, currentVert, nextVert, &intersectionPoint);
                          if (DoesPlaneIntersect)
                          {
                              newFace.Add(IntersectionPoint);
                              break;
                          }
                          else
                          {
                              newFace.Add(nextVert);
                          }
                      }

                      if (!PointIsInCell) newFace = GetReverseCell();

                      newCell.Add(newFace);
                  }
                
            }
            cell = newCell;
      }
      TrianglulateCell();
      CreateDisplayCell();
}
```
Figure 1


## Optimisation 

### CPU

Currenty, the major bottleneck to the program is creating new fracture objects for rendering, taking 83.36% of the processing time. This is shown in figure 2

<img width="722" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/4b6eb04a-a471-4423-aaf7-186378811aa3" />

Figure 2

After investigating why, it has been discovered that it is because of two major points, shader creation and buffering data, as shown in figures 3 and 4.

<img width="433" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/db70cdfe-4b7a-446a-bf33-7101303d1046" />

Figure 3

<img width="782" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/e143085c-5463-4752-b6cd-b114e6ceb3cd" />

Figure 4

To solve this, The shader could be created once for the class, but the problem is that currently the shader class allocates uniform buffers on a per shader basis, so the shader class would need to be re-written to allocate one large buffer that is used per class, saving on multiple shader creation. We could do something similar with vulkan memeory allocation, and allocate once in bulk beforehand

### Memory

<img width="311" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/62745c86-1aeb-4601-93c9-e22964aeab18" />

Figure 5

<img width="946" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/98e1beb9-7faf-4364-82d7-a0e0f6d1d371" />

Figure 6

Figure 5 shows that the algorithm is allocating 1.217 mb more memory than before the call. This is not a significant amount of memory, but it could be reduced. As figure 6 shows, the main bottleneck is creating FracturePieces, the reson being they are allocated on the heap. Significant memory could be saved by allocating them on the stack instead. This will be one of the aims for the next interation. 

## References

[1] Georges Vorono¨ı. New applications of continuous parameters to the
theory of quadratic forms. Z. Reine Angew. Math, 134:198, 1908.

[2] Vulkan, ‘Home | Vulkan | Cross platform 3D Graphics’. Accessed: Oct. 25, 2025. [Online]. Available: https://vulkan.org/

[3] ‘OpenGL - The Industry Standard for High Performance Graphics’. Accessed: Oct. 25, 2025. [Online]. Available: https://opengl.org/

[4] A. Blackert, Evaluation of Multi-Threading in Vulkan. 2016. Accessed: Oct. 25, 2025. [Online]. Available: https://urn.kb.se/resolve?urn=urn:nbn:se:liu:diva-132025

[5] A. Okabe, B. Boots, K. Sugihara, S. N. Chiu, and D. G. Kendall, ‘Spatial Tessellations: Concepts and Applications of Voronoi Diagrams, Second Edition’.

[6] Edgar Programmator, A simple algorithm for 2D Voronoi diagrams, (Sept. 21, 2023). Accessed: Oct. 25, 2025. [Online Video]. Available: https://www.youtube.com/watch?v=I6Fen2Ac-1U

[7] D.-M. Yan, W. Wang, B. Lévy, and Y. Liu, ‘Efficient computation of clipped Voronoi diagram for mesh generation’, Computer-Aided Design, vol. 45, no. 4, pp. 843–852, Apr. 2013, doi: 10.1016/j.cad.2011.09.004.

[8] D. T. Lee and B. J. Schachter, ‘Two algorithms for constructing a Delaunay triangulation’, International Journal of Computer and Information Sciences, vol. 9, no. 3, pp. 219–242, June 1980, doi: 10.1007/BF00977785.


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
