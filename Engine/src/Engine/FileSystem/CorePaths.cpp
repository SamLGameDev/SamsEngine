#include "CorePaths.h"
#include <filesystem>

FPath CorePaths::Contents;

CorePaths::CorePaths()
{
	std::filesystem::path currentDir = std::filesystem::current_path();

	//Find the root directory by looking for Root.SE, then set the contents path

	while (!std::filesystem::exists(currentDir / "Root.SE"))
	{
		currentDir = currentDir.parent_path();
	}

	Contents.Path = currentDir.string() + "\\Contents";

}
