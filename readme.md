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

Camera.cpp and .h except for the start function

Array.h and .cpp except except the not equally comparable, Add, emplace, Reallocate, ReSize, Copy and MakeNewArray.

BaseDelegate.h and .cpp

Matrix.h and .cpp

Vector2D.cpp and .h

Vector3D.cpp and .h  except for NumericMin and NumericMax, GetLineIntersectionPointWithPlane, Min, Max, GetSingedDistance, OrderByAngle, GetPlaneAxis, GetPlaneNormal, AnglePointPair

ErrorCodes.cpp and .h

CorePaths.cpp and .h

Path.cpp and .h

Voronoi.cpp and .h except FracturePlaneRandom, CreateMeshFractureGPU, FracturePlaneRandomGPU, GenerateRandomPointsInBounds, GenerateNewPointSets, FracturePeiceGPU, RawCell, FixedSizeFace, Cell,
WorkingBuffer, VOut, VOutRaw TetFace, FTet, InTets, VoronoiSSBOIn, TetrahedrliseMesh

Voronoi2D.cpp and .h

BaseDataBuffers.cpp and .h except GenerateShaderStorageBuffer, BindShaderStorageBuffer, MapBufferMemory, UnMapBufferMemory, RemoveBuffer

DataBuffers.cpp and .h except GenerateShaderStorageBuffer, BindShaderStorageBuffer, MapBufferMemory, UnMapBufferMemory, RemoveBuffer

DataBuffersVulkan.cpp and .h except GenerateShaderStorageBuffer, BindShaderStorageBuffer, MapBufferMemory, UnMapBufferMemory, RemoveBuffer

InitialiseVulkan.cpp and .h

RuntimeEngineVulkan.cpp and .h

VulkanFrameBuffer.cpp and .h

VulkanGraphicsCard.cpp and .h

VulkanImageView.cpp and .h

VulkanInstance.cpp and .h

VulkanLogicalDevice.cpp and .h except GetComputeQueue, ComputeQueue

VulkanRenderPass.cpp and .h

VulkanRenderPipeline.cpp and .h except ErrorCodes URenderPipeline::Init(UVulkanComputeShader& InShader, const Array<VkDescriptorSetLayoutBinding>& Descriptors)

VulkanSubsytemInitialiser.cpp and .h

VulkanSwapChain.cpp and .h

InputAction,cpp and .h

InputActionMouse.cpp and .h

InputManager.cpp and .h

MathCore.cpp and .h

SmallestCircle.cpp and .h

Mesh.cpp and .h except SetUpMesh

Model.cpp and .h except CalculatePointsForMesh

Transform.cpp and .h

World.cpp and .h

WorldObject.cpp and .h

Object.cpp and .h

BaseShader.cpp and .h

Shader.cpp and .h

VulkanShader.cpp and .h except  CreateDefaultShaderFile, CreateDefaultFragmentFile, CreateDefaultGeometryFile.

BaseTexture.cpp and .h

Texture.cpp and .h

VulkanTexture.cpp and .h

BaseRenderer.cpp and .h except AddFracture(GracturePeiceGPU*)

InterfaceRenderer.cpp and .h except AddFracture(GracturePeiceGPU*)

VulkanRenderer.cpp and .h

DelaunayTriangulation.cpp and .h

VulkanWindow.cpp and .h

Window.cpp and .h

## Description

This project has implemented a 3D voronoi fracturing algorithm [1] on the GPU through compute shaders on both Vulkan[2] and OpenGL[3]. This project uses half-plane clipping [4][5] to generate a Voronoi diagram, and tetrahedralises a mesh using constrained Delaunay triangulation[6] that is then used to clip the cell. The CDT used comes from CGAL, a geometry library[7]. 

## Methodology
Cell generation is implimented using half plane clipping, as shown in fig 1.

```C++
Array<Vector3D> points = GetPointsForThisSet();
for (size_t i = 0; i < points.GetSize(); i++)
{
      Vector3D currentPoint = points[i];
      Array<Array<Vector3D>> cell = GetAllFacesOfBoundingBox();
      for (size_t j = 0; j < points.GetSize(); j++)
      {
            if (i == j) continue;
            Vector3D comparedPoint = points[j];
            Plane perpendicularBisector = GetPerpendicularBisector(currentPoint, comparedPoint);
            Array<Array<Vector3D>> newCell;
            for (Array<Vector3D> Face : cell)
            {
                 newCell.Add(CutFaceByPlane(perpendicularBisector)); 
            }
            OrderCell(newCell);
            cell = newCell;
      }
      TrianglulateCell();
      CreateDisplayCell();
}
```
Figure 1

This algorithm works by cutting a cube by a perpendicular bi-sector plane of every other point to generate a fracture piece. It repeats this for all points to geneerate a full Voronoi diagram of the mesh. 

Figure 2 shows the process of clipping a cell to a tetrahedralised mesh.

```C++

Cell cell = GetCell();
FBox cellBox =  GetCellBoundingBox(cell);
Array<Tetrahedra> tets = GetTets();
Cell NewCell;

for (uint i = 0; i < tets.GetSize(); i++)
{
    Tetrahedra tet = tets[i];
    if (!IsBoxIntersectingBox(cellBox, FBox(tet)) continue;

    Vector3D cellCenter = cell.GetCenter();
    for (const Face& face : cell.Faces)
    {
        tet = CutTetByFace(face, tet);
        OrderTet(tet);
    }

    if (tet.NumFaces == 0) continue;
    NewCell.AppendTet(tet);
}
```

This algorithm works by iterating through each tetrahedron of the mesh, performing an AABB test to check if it is within the bounds of the cell, and if it is, Clips the tetrahedron by the cell. Once fully clipped, if it still has faces, it is added to the new cell. After iterating through all tetes, a new clipped cell is formed.

## Documentation

Find the doxygen generated documentation below

https://samlgamedev.github.io/samsenginedocs.github.io/

## Video


https://github.falmouth.ac.uk/user-attachments/assets/d35caa09-7ed5-4391-865a-00898ac367f4



## References

[1] Georges Vorono¨ı. New applications of continuous parameters to the
theory of quadratic forms. Z. Reine Angew. Math, 134:198, 1908.

[2] Vulkan, ‘Home | Vulkan | Cross platform 3D Graphics’. Accessed: Oct. 25, 2025. [Online]. Available: https://vulkan.org/

[3] ‘OpenGL - The Industry Standard for High Performance Graphics’. Accessed: Oct. 25, 2025. [Online]. Available: https://opengl.org/

[4] A. Okabe, B. Boots, K. Sugihara, S. N. Chiu, and D. G. Kendall, ‘Spatial Tessellations: Concepts and Applications of Voronoi Diagrams, Second Edition’.

[5] Edgar Programmator, A simple algorithm for 2D Voronoi diagrams, (Sept. 21, 2023). Accessed: Oct. 25, 2025. [Online Video]. Available: https://www.youtube.com/watch?v=I6Fen2Ac-1U

[6]L. P. Chew. 1987. Constrained Delaunay triangulations. In Proceedings of the third annual symposium on Computational geometry (SCG '87). Association for Computing Machinery, New York, NY, USA, 215–222. https://doi.org/10.1145/41958.41981

[7] ‘The Computational Geometry Algorithms Library’. Accessed: Mar. 14, 2026. [Online]. Available: https://www.cgal.org/


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

## How to run the Build
First, download the build from the releases section. The one to download it Release.zip
Then, unzip it, and go to Release->Windows-Release->src and run SamsEngine.exe. 

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
* CGAL
