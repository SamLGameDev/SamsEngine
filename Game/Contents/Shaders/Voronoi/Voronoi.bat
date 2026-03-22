%VULKAN_SDK%/Bin/glslc.exe VoronoiCellGeneration.comp -o VoronoiCellGeneration.spv 
%VULKAN_SDK%/Bin/glslc.exe VoronoiClipping.comp -o VoronoiClipping.spv
%VULKAN_SDK%/Bin/spirv-val.exe VoronoiCellGeneration.spv
%VULKAN_SDK%/Bin/spirv-val.exe VoronoiClipping.spv
%VULKAN_SDK%/Bin/glslangValidator -V VoronoiCellGeneration.comp -o VoronoiCellGeneration.spv
pause