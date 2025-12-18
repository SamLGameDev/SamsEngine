## Table of contents
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#do-not-mark">DO NOT MARK</a>
    </li>
        <li>
        <a href="#description">Description</a>
    </li>
        <li>
        <a href="#methodology">Methodology</a>
    </li>
        <li>
        <a href="#optimisation">Optimisation</a>
           <ul>
        <li><a href="#cpu">CPU</a></li>
        <li><a href="#memory">Memory</a></li>
        <li><a href="#gpu">GPU</a></li>
      </ul>
    </li>
         <li>
        <a href="#references">References</a>
    </li>
    <li>
        <a href="#documentation">Documentation</a>
    </li>
    <li>
        <a href="#video">Video</a>
    </li>
    <li><a href="#building-this-project">Building this project</a></li>
        <li><a href="#controls">Controls</a></li>
    <li><a href="#dependencies">Dependencies</a></li>
  </ol>
</details>


## DO NOT MARK
StaticMeshComponent.cpp and h
Item.cpp and h
LinkedList.cpp and h
Verticie.cpp and h
BaseLight.cpp and h
DirectionalLight.cpp and h
LightManager.cpp and h
PointLight.cpp and h
SpotLight.cpp and h

All of wire shapes.cpp except lines 139 -190. This is marked by the DO mark comments.
All of wireshapes.h

ObjectFactory.cpp and h

CollisionCast.cpp and h

FrameBufferFactory.cpp and h
UnifromBufferFactory.cpp and h

OpenGLShader.cpp and h

CreateDefaultShaderFile, CreateDefaultGeometryFile, CreateDefaultFragmentFile from VulkanShader.cpp
CreateDefaultShaderFile, CreateDefaultGeometryFile, CreateDefaultFragmentFile from VulkanShader.h

Renderer.cpp
Renderer.h

CubeMap.cpp and h
OpenGLTexture.cpp and h

FirstWindow.cpp and h

RuntimeEngine.cpp and h

SubsystemInitialiser.cpp and h

MeshObject.cpp and h

## Description

This project aims to implement a 3D voronoi fracturing algorithm [1] using vulkan[2]. Vulkan has been chosen over alternatives like OpenGL[3] due to its ability to take advantage of the parallelism of modern GPU's [4], which will be cruicial in this project as there will be many points to calculate which can be calculated independently of one another. A triangulation algorithm will also be needed to generate a mesh from the fractured points, and as such delaunay triangulation [8] will be used. This should result in significant performance gains. For future work, this project would aim at implimenting a clipped voronoi diagram[7], allowing the fracturing of a complex 3D mesh. 

As stated in the previous worksheet, this iteration aimed to impliment a 3D delaunay triangulation to fix the clipping issue with the current iteraction. This was successfully done, but unfortunatly did not result in a fixed diagram. This is because delaunay does not retain the previous shape of the input verticies, resulting in the same clipping as before. The solution to this is to impliment constrained elaunay triangulation, but unfortunaly the remaining time is too short for such an implimentation. However, this does not mean the porblem can not be solved inellegantly, and as such, a simple triangulation algorithm has been implimented on each face of the fracture, before combining them to recieve similar results as CDT. 

For additional interactability, controls using the arrow keys have been implimented to show a fracture level for the mesh, allowing a more detailed view of the diagram. However, this lead me to discover that my current algorithm has failed to properly impliment a voronoi diagram, as whilst the outside appears fine, the inside fractures are not cut properly, and are missing faces/misformed. A deeper investigation has lead to the discovery that this is due how intersect faces are added after a slice, being added last, which is out of order. Even fixing this ordering issue however did not fix it, leading me to believe that this is due to the intersect face needing to be modified for all faces. This would be hard to impliment for my algorithm, leading me to believe my current algorithm is fundementally wrong. After reading up on this, i have decided to attemp to impliment a different voronoi algorithm using delaunay triangulation. 

The new method and old method will both be displayed, first the old method will be run, then on application close, the second improved method will run.
CGAL was used in the development of this new version, to first ensure the underlying delaunay was correct, before my own version was implemented.

## Methodology
The old method is implimented by adapting the half plane clipping algorithm for 3D [5][6]. The pesudocode for that can be found in fig 1.

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

This algorithm works by cutting a cube by a perpendicular bi-sector plane of every other point to generate a fracture piece. It repeats this for all points to geneerate a full Voronoi diagram of the mesh. 

The improved method works by extracting the Voronoi diagram from a Delaunay triangulation [9], and clipping it to the bounding box.

A Delaunay triangulation is a geometric dual to a Voronoi diagram, which means one can be extracted from the other. This works by first computing the delaunay triangulation of a set of points.
A Voronoi vertex is then the center of the circumsphere of a tetrahedron.

A Voronoi Edge is two voronoi vertecies that share a face of a tetrahedron.

A Voronoi Face is all the voronoi verticies that are dual to the tetrahedra which are incident to delaunay edge.

A Voronoi Cell is then all voronoi verticies which are dual to the tetrehedra incident to the current Point. The point is one of the random points generated at the start of voronoi generation.

Thus, using this, we can extract the Voronoi cell of all Points. However, this still needs clipping to the box, and missing faces need to be added. Thus a sutherland-hodgeman [10] algorithm has been implimented to
clip each cell to the bounding box. It has been modified to also add any missing faces that arent included in the intersection.

```C++
Array<Tetrahedron> Tetrahedra = GetSuperTetrahedron(Vertices);
for (const auto& point : Points)
{
  Array<Tetrahedron> newTetrahedron;
  Array<Face> TetsToRemove;
  
  for (const auto& tetrahedron : Tetrahedra)
  {
  
    if (!tetrahedron.IsPointInCircumSphere(point))
    {
      newTetrahedron.Add(tetrahedron);
      continue;
    }
    for (const auto& face : tetrahedron.faces)
    {
      TetsToRemove.Add(face);
    }
  }
  TetsToRemove.GetFacesThatOnlyAppearOnce();
  
  for (const auto& f: boundaryFaces)
  {
    newTetrahedron.Add(Tetrahedron(f.Vertices[0], f.Vertices[1], f.Vertices[2], point));
  }
  Tetrahedra = newTetrahedron;
}
```

Figure 10, This is a Bowyer-Watson delaunay triangulation [11].

```C++
for (const auto& point : Points)
{
  Array<Tetrahedron> tets = GetAllIncidentTets(point);
  Array<Edge> edges = GetAllEdgesUnique(tets, point);
  Array<Face> faces;
  for (const auto& edge : edges)
  {
    Array<Tetrahedron> ring = GetAllTetsThatContainEdge(edge, tets);
    Array<Vector3D> voronoiVertices = GetAllVoronoiVertices(ring);
    faces.Add(GetFaceFromOrderedVertices(voronoiVertices, edge);
  }
  ClipCellToBoundingBox(faces);
  DrawCell(faces);
}
```

Figure 11, Commputing a voronoi diagram from a delaunay triangulation

## Optimisation 

### CPU

Currenty, the major bottleneck to the program is slicing the shape by the plane. This is taking up 81.84% of the program time, as shown in figure 2

<img width="257" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/1a7da11c-c6a3-476a-8d25-9c530e362c1b" />

Figure 2

After investigating why, it has been discovered that it is because of two major points, 13% with copying the face into the new faces array (fig 3), and 46.43% spend on adding a face to the array (fig 4).
This could be mitigated by pre-allocating the array, saving on the amount of new used, saving significant time. Parrallel processing would also help, as each fracture is unrealted to each other, meaning they can be executed in parrallel.

<img width="254" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/cdb3e2ca-ad1c-4e50-b90b-2360f0889b79" />

Figure 3

<img width="253" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/e654f470-7b16-40e9-9c71-cdc9e215791e" />

Figure 4

### Memory

<img width="310" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/d683824f-e25d-4520-ab50-75ba3109e604" />


Figure 5

<img width="781" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/cd45806f-ba5c-46f5-ac49-88b6c1ff1af1" />

Figure 6

Figure 5 shows we are allocating around 2.48 mb of extra memory when running the program. Most of the memory being allocted comes from the model class, being from the stored vertices. This could be reduced by not keeping track of stored vertices after buffering them, as that is no longer neccessary.

### GPU

<img width="896" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/4f491712-509c-44b4-afc8-45c6a5012103" />

Figure 7

As shown in figure 7, there is a period of gpu activity at the start, before stopping for around 3 seconds, before it picks up again. This suggests out program has a lull of GPU inactivity, which could potentially be used for compute shaders, speeding up the processing speed of the program. The usage as a whole is very low, suggesting gpu usage isnt a bottleneck.

<img width="440" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/98a4cefd-97d0-4f6a-bcb5-0873b44c6c6e" />

Figure 8

As shown in figure 8, variables are as expected, showing data is being passed to the GPU correctly.

<img width="434" alt="image" src="https://github.falmouth.ac.uk/user-attachments/assets/c68d6704-88ea-4a92-bf7b-0b9b673c2b23" />

Figure 9

As figure 9 shows, the fragment is malformed, not forming a convex shape. This will be fixed with the new method.

## Documentation

Find the docygen generated documentation below

https://samlgamedev.github.io/samsenginedocs.github.io/

## Video

Plane Bi-secting method. This is the broken one.

https://github.falmouth.ac.uk/user-attachments/assets/c6b487de-a8e4-432b-a18b-cb7c080ff8ed

Delaunay extraction method, Works perfectly! 

https://github.falmouth.ac.uk/user-attachments/assets/37f4714d-12af-48df-8fc1-39d4b4927bc4

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

[9] H. Ledoux, "Computing the 3D Voronoi Diagram Robustly: An Easy Explanation," 4th International Symposium on Voronoi Diagrams in Science and Engineering (ISVD 2007), Glamorgan, UK, 2007, pp. 117-129, doi: 10.1109/ISVD.2007.10.
keywords: {Robustness;Algorithm design and analysis;Programming profession;Data mining;Geographic Information Systems;Distributed computing;Computational geometry;Sorting;Application software;Data structures},

[10] I. E. Sutherland and G. W. Hodgman, ‘Reentrant polygon clipping’, Commun. ACM, vol. 17, no. 1, pp. 32–42, Jan. 1974, doi: 10.1145/360767.360802.

[11] A. Bowyer, ‘Computing Dirichlet tessellations*’, Comput J, vol. 24, no. 2, pp. 162–166, Jan. 1981, doi: 10.1093/comjnl/24.2.162.


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

##What to open
You should open the Game folder with cmake. This wll contain the worksheets runable code, as all engine contains are unit tests. To run unit tests open engine and run project-unit.exe
Please close the application using the red X first, as the second application containing the imporved version will then run.

## Controls

WASD to move

Mouse to look around

Left arrow to explode the shape.

Right arrow to condese the shape.

## Dependencies
* glfw3
* vcpkg
* cmake
* glad
* stb
* glm
* assimp
* nlohmann json
* OpenGL
