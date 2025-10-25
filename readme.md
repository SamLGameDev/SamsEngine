
## Description

This project aims to impliment a 3D voronoi fracturing algorithm [1] using vulkan[2]. Vulkan has been chosen over alternatives like OpenGL[3] due to its ability to take advantage of the parallelism of modern GPU's [4], which will be cruicial in this project as there will be many points to calculate which can be calculated independently of one another. A triangulation algorithm will also be needed to generate a mesh from the fractured points, and as such delaunay triangulation [8] will be used. This should result in significant performance gains. For future work, this project would aim at implimenting a clipped voronoi diagram[7], allowing the fracturing of a complex 3D mesh.

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

Currently, there are no major bottlenecks for the program, but here are some issues that will help the next 3D iteration run faster.

![image](https://github.falmouth.ac.uk/user-attachments/assets/347632b3-4130-4008-ba24-58fd7d0afbf3)
Figure 4

Firgure 4 shows that creating the point shader is very inefficient, so for the next iteraction, this will be moved out of fracture piece creation, so its only ever created once.

![image](https://github.falmouth.ac.uk/user-attachments/assets/3f76fc17-909c-440b-929f-0a0f5b9fce8f)
Figure 5

Fig 5 shows assinging to the heap for each fracrue peice is very inefficent, so the code will be rewritten so it can be stack based

![image](https://github.falmouth.ac.uk/user-attachments/assets/e75faf0a-7292-465f-a9bd-3c1b207ce0ac)
Figure 6

Fig 6 shows copying the newcell to cell is inefficient, so the program will be rewritten to use newcell instead, avoiding the copying.

![image](https://github.falmouth.ac.uk/user-attachments/assets/f1104a81-8001-4c11-8860-a3f5e65248ea)
Figure 7

Fig7 shows adding to the array is expensive, so the add method will be rewritten to allocate space to double the current size when running out, avoiding so many new allocations

Currently, the program could be imporved by taking advantage of parrallel programming, allocating different threads for each point. This will help improve the performance of the 3D application.

### Memory

<img width="308" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/c3b5db15-025f-401a-8b0b-ca7507daca16" />
Figure 2

<img width="683" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/4841c69b-659c-486a-8323-4d6dbb68056f" />
Figure 3

Fig 2 shows that the current 2D algorithm is using more memory than it should be. Figure 3 shows that this is because of the fracture pieces Being created. For the next iteration, the aim will be to reduce the size of this class, and change it to a stack allocation instead of a heap one.

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
