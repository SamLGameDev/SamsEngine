#include "FileSaving.h"

#include <fstream>

#include "CorePaths.h"
#include "Vector2D.h"
#include "CGAL/basic.h"

void UFileWriter::SaveArray(const std::string_view& Name, const Array<Vector3D>& Data)
{
	const std::string directory = CorePaths::Contents.Path + Name.data();

	std::ofstream Save(directory, std::ios::binary);

	size_t count = Data.GetSize();
	Save.write(reinterpret_cast<const char*>(&count), sizeof(count));
	Save.write(reinterpret_cast<const char*>(Data.GetFirstPtr()), count * sizeof(Vector3D));

}

void UFileWriter::Load(const std::string_view& Name, Array<Vector3D>& Data)
{
    const std::string directory = CorePaths::Contents.Path + Name.data();
	std::ifstream in(directory, std::ios::binary);

    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    Data.Reallocate(count);
    in.read(reinterpret_cast<char*>(Data.GetFirstPtr()), count * sizeof(Vector3D));
}

void UFileWriter::Load(const std::string_view& Name, Array<Vector3D>& Data, const Vector2D& Range)
{
    const std::string directory = CorePaths::Contents.Path + Name.data();
    std::ifstream in(directory, std::ios::binary);

    if (!in) std::cerr << "Could not open file" << directory << std::endl;
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    Data.Reallocate((Range.Y - Range.X) + 1);
    in.seekg(sizeof(count) + Range.X * sizeof(Vector3D), std::ios::beg);
    in.read(reinterpret_cast<char*>(Data.GetFirstPtr()), (Range.Y - Range.X) * sizeof(Vector3D));
}


std::string UFileWriter::ReadFileContents(const std::string_view& Location) 
{
	std::ifstream File;
	File.open(Location.data());
	std::stringstream Buffer;
	Buffer << File.rdbuf();
	File.close();
	
	std::string text = Buffer.str();

	return text;
}
