#include <cassert>
#include <gtest/gtest.h>
#include "MathCore.h"
#include "Vector3D.h"
#include "Array.h"
#include "BaseDelegate.h"
#include <functional>
#include "Vector2D.h"
#include "CorePaths.h"
#include "DelaunayTriangulation.h"
#include "HardwareDetails.h"
#include "RuntimeEngine.h"
#include "Voronoi2D.h"
#include "RuntimeEngineVulkan.h"
#include "VoronoiClipping.h"
#include <string>

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(Test1, Math)
{
	bool expected = true;
	bool result = MathCore::IsEven(2);
	ASSERT_EQ(expected, result);
}



TEST(Array, Allocation)
{
	Array<int> test;

	test.Add(5);

	test.Add(3);

	test.Remove(0);

	test.Remove(5);

	ASSERT_EQ(3, test[0]);
}

TEST(Array, Searching)
{
	Array<int> test;

	test.Add(5);

	test.Add(3);

	size_t index;
	bool btest = test.Contains(5, index);

	ASSERT_EQ(btest, true);
	ASSERT_EQ(0, index);

	test.Contains(3, index);
	ASSERT_EQ(1, index);

	btest = test.Contains(-1, index);

	ASSERT_EQ(btest, false);
	ASSERT_EQ(1, index);
}

TEST(Array, Move)
{
	// Arrange two faces, each with a small vertex list
	Face fa;
	Face fb;
	Face fc;

	fa.Vertices = Array<Vector3D>{ Vector3D(1,0,0), Vector3D(0,1,0) };
	fb.Vertices = Array<Vector3D>{ Vector3D(0,0,1), Vector3D(-1,0,0) };
	fc.Vertices = Array<Vector3D>{ Vector3D(0,0,0), Vector3D(-1,0,0), Vector3D(2, 53, 43) };

	const size_t aBefore = fa.Vertices.GetSize();
	const size_t bBefore = fb.Vertices.GetSize();
	const size_t cBefore = fc.Vertices.GetSize();

	// Act: append fb's vertices into fa
	fa.Vertices.Emplace(std::move(fb.Vertices));

	// Assert combined contents and size (order preserved)
	ASSERT_EQ(fa.Vertices.GetSize(), aBefore + bBefore);
	ASSERT_EQ(fa.Vertices[0], Vector3D(1, 0, 0));
	ASSERT_EQ(fa.Vertices[1], Vector3D(0, 1, 0));
	ASSERT_EQ(fa.Vertices[2], Vector3D(0, 0, 1));
	ASSERT_EQ(fa.Vertices[3], Vector3D(-1, 0, 0));

	fa.Vertices.Emplace(std::move(fc.Vertices));

	ASSERT_EQ(fa.Vertices.GetSize(), aBefore + bBefore + cBefore);
	ASSERT_EQ(fa.Vertices[0], Vector3D(1, 0, 0));
	ASSERT_EQ(fa.Vertices[1], Vector3D(0, 1, 0));
	ASSERT_EQ(fa.Vertices[2], Vector3D(0, 0, 1));
	ASSERT_EQ(fa.Vertices[3], Vector3D(-1, 0, 0));
	ASSERT_EQ(fa.Vertices[4], Vector3D(0, 0, 0));
	ASSERT_EQ(fa.Vertices[5], Vector3D(-1, 0, 0));
	ASSERT_EQ(fa.Vertices[6], Vector3D(2, 53, 43));
	ASSERT_EQ(bBefore, 2);
}


TEST(LinkedList, Allocation)
{
	LinkedList<int> test;

	test.Add(5);

	test.Add(3);

	test.Remove(0);

	test.Remove(5);

	ASSERT_EQ(3, test[0]);

}

TEST(LinkedList, Searching)
{
	LinkedList<int> test;

	test.Add(5);

	test.Add(3);

	unsigned int index;
	bool btest = test.Contains(5, index);

	ASSERT_EQ(btest, true);
	ASSERT_EQ(0, index);

	test.Contains(3, index);
	ASSERT_EQ(1, index);

	btest = test.Contains(-1, index);

	ASSERT_EQ(btest, false);
	ASSERT_EQ(1, index);
}

TEST(LinkedList, Copying)
{
	LinkedList<int> test;

	test.Add(5);

	test.Add(3);

	LinkedList<int> copy = test;

	bool btest = copy == test;

	ASSERT_EQ(btest, true);

	copy.Remove(3);
	btest = copy == test;

	ASSERT_EQ(btest, false);

	copy.Add(5);
	btest = copy == test;

	ASSERT_EQ(btest, false);

}

TEST(Vector2D, Creation)
{
	Vector2D test;

	ASSERT_EQ(test, Vector2D::Zero);

	test = Vector2D(1, 6);

	ASSERT_EQ(test.X, 1);
	ASSERT_EQ(test.Y, 6);

}

TEST(Vector2D, copying)
{
	const Vector2D test = Vector2D(1, 6);

	const Vector2D copy = test;

	ASSERT_EQ(test, copy);

}


TEST(Vector3D, Creation)
{
	Vector3D test;

	ASSERT_EQ(test, Vector3D::Zero);

	test = Vector3D(1, 6, 9);

	ASSERT_EQ(test.X, 1);
	ASSERT_EQ(test.Y, 6);
	ASSERT_EQ(test.Z, 9);

}


TEST(Vector3D, copying)
{
	const Vector3D test = Vector3D(1, 6, 9);

	const Vector3D copy = test;

	ASSERT_EQ(test, copy);

}


TEST(Vector3D, Operators)
{
	Vector3D test = Vector3D(5, 5, 5);

	ASSERT_EQ(test / 5, Vector3D::One);

	ASSERT_EQ(test / Vector3D(2, 3, 1), Vector3D(2.5f, 1.666666667f, 5));

	Vector3D negTest = Vector3D(-5, -5, -5);

	ASSERT_EQ(-test, negTest);

	ASSERT_EQ(test - Vector3D(1.3f, 8, 5), Vector3D(3.7f, -3, 0));

	ASSERT_EQ(test * 2, Vector3D(10, 10, 10));
	ASSERT_EQ(test * Vector3D(1.3f, 8, -5), Vector3D(6.5f, 40, -25));

	ASSERT_EQ(test + 3, Vector3D(8, 8, 8));
	ASSERT_EQ(test + Vector3D(1.3f, 8, -5), Vector3D(6.3f, 13, 0));

	test += 8;

	ASSERT_EQ(test, Vector3D(13, 13, 13));

	test -= 8;

	ASSERT_EQ(test, Vector3D(5, 5, 5));

	ASSERT_EQ(test < 6, true);

	ASSERT_EQ(test < Vector3D::One, false);

	ASSERT_EQ(test > 9, false);

	ASSERT_EQ(test > Vector3D::One, true);


}

TEST(Vector3D, Cross)
{
	Vector3D num = Vector3D(5, 5, 5);

	Vector3D::Clamp(num, Vector3D(0, 0, 0), Vector3D(1, 1, 1));

	const Vector3D expected = Vector3D(1, 0, 0);

	const Vector3D right = Vector3D::Cross(Vector3D(0, -1, 0), Vector3D(1, 0, 0));

	const Vector3D Up = Vector3D::Cross(right, Vector3D(0, -1, 0));

	ASSERT_EQ(Up, expected);
}

TEST(Vector3D, Normalisation)
{
	const Vector3D test = Vector3D(5, 5, 5).Normalised();

	ASSERT_EQ(test, Vector3D(0.57735026f, 0.57735026f, 0.57735026f));

}

TEST(Vector3D, Length)
{
	const Vector3D test = Vector3D(5, 5, 5);

	ASSERT_EQ(test.GetLength(), 8.660254038f);

	ASSERT_EQ(test.GetSquaredLength(), 75);
}

TEST(Vector3D, Random)
{
	const Vector3D test = Vector3D::RandomRange(Vector3D::One, Vector3D(9, 9, 9));

	const bool bWithin = Vector3D::WithinRange(test, Vector3D::One, Vector3D(9, 9, 9));

	ASSERT_EQ(bWithin, true);
}

TEST(Vector3D, Dot)
{
	const Vector3D test = Vector3D(3, 9, 1);

	const float Dot = Vector3D::Dot(test, Vector3D(5, 3, 2));

	ASSERT_EQ(Dot, 44);
}

TEST(Vector3D, Absolute)
{
	const Vector3D test = Vector3D(-3, 9, -1);

	ASSERT_EQ(test.Abs(), Vector3D(3, 9, 1));
}

TEST(Vector3D, AlmostEqual)
{
	const Vector3D test = Vector3D(-3, 9, -1);

	ASSERT_EQ(Vector3D::IsAlmostEqual(test, Vector3D(-3.00000003f, 9.00000002f, -1.00000000006f)), true);
}


TEST(Vector3D, PlaneLineIntersection)
{
	const Vector3D test = Vector3D(-3, 9, 1);

	Vector3D out;

	const bool bHit = Vector3D::GetIntersectionPointWithPlane(Vector3D::Zero, Vector3D(0, 0, 1), test, Vector3D(0, 0, -1), out);

	ASSERT_EQ(bHit, true);

	ASSERT_EQ(out, Vector3D(-3, 9, 0));
}


char* itoa(int Value, int Base)
{
	//convert to long long to handle INT_MIN
	long long extendedVal = Value;

	//Negate the value so maths works, and we know to add a sign
	bool negate = false;
	if (Value < 0)
	{
		extendedVal = -extendedVal;
		negate = true;
	}

	//Base 1 is an edge case, as it will cause an infinite loop otherwise, as val / 1 == val
	if (Base == 1)
	{
		char* valueAsChar = new char[extendedVal+1 + negate];

		//always set negative, will be overriden if not negative
		valueAsChar[0] = '-';
		for (unsigned int i = 0 + negate; i < extendedVal + negate; i++)
		{
			valueAsChar[i] = '1';
		}
		valueAsChar[extendedVal + negate] = '\0';
		return valueAsChar;
	}

	unsigned int bufferSize = 0 + negate;
	long long copyVal = extendedVal;

	do
	{
		copyVal /= Base;
		bufferSize++;
	} while (copyVal > 0);

	//add one more to buffer size for null terminator

	char* valueAsChar = new char[bufferSize+1];
	unsigned int index = bufferSize - 1;
	do 
	{
		const long long val = extendedVal % Base;

		//Start from 0 if a less than 10, or from A if greater
		// will work up to Base 36, but cannot guarantee Character set ordering from there.

		valueAsChar[index] = val > 9 ? (val - 10) + 'A' : val + '0';

		extendedVal /= Base;

		index--;
	}while (extendedVal > 0);

	if (negate)
	{
		valueAsChar[0] = '-';
	}

	valueAsChar[bufferSize] = '\0';

	return valueAsChar;
}

void BuildStringFromMatrix(int* Matrix, int NumRows, int NumColumns,
	char* OutBuffer)
{
	std::string output;

	int left = 0;
	int right = NumColumns - 1;
	
	int top = 0;
	int bottom = NumRows - 1;

	auto getMatrixIndexAsNum = [&](const size_t& Row, const size_t& Col)
		{
			return std::to_string(Matrix[Row * NumColumns + Col]);
		};
	
	auto addToOutput = [&](const size_t& Row, const size_t& Col)
		{
			output += ", " + getMatrixIndexAsNum(Row, Col);
		};

	//bring the borders closer each iteration, until they pass each other
	while (top <= bottom && left <= right)
	{

		for (int i = left; i <= right; i++)
		{
			if (output.empty())
			{
				output = getMatrixIndexAsNum(top, i);
			}
			else
			{
				addToOutput(top, i);
			}
		}
		top++;
		for (int i = top; i <= bottom; i++)
		{
			addToOutput(i, right);
		}
		right--;

		if (top <= bottom) {

			for (int i = right; i >= left; i--)
			{
				addToOutput(bottom, i);

			}
		}
		bottom--;
		if (left <= right) {
			for (int i = bottom; i >= top; i--)
			{
				addToOutput(i, left);
			}
		}
		left++;
		

	}

	output += '\0';
	memcpy(OutBuffer, output.data(), output.size());
}

TEST(Tringulation, Delauney)
{

////	Tetrahedron tet = { {1, 1, 1}, {-1, -1, 1}, {-1, 1, -1}, {1, -1, -1} };
////	
////	DelaunayTriangulation tri;
////
////	Array<Vector3D> verts= {
////	{0,0,0},
////	{1,0,0}, 
////	{0,1,0},
////	{0,0,1},
////	{1,1,1} 
////	};
////
////	Array<uint16_t> inds;
////
//////	tri.Triangulate(verts, inds);
////	
////	for (const auto& index : inds)
////	{
////		std::cout << index << ", ";
////	}
////
////
////	ASSERT_EQ(tet.IsPointInCircumSphere({ 0.1, 0, 0 }), true);
////
////	ASSERT_EQ(tet.IsPointInCircumSphere({ 5, 0, 0 }), false);
////
////	ASSERT_EQ(tet.IsPointInCircumSphere({ 0.9, 1, 1 }), true);
}

void RunEngine(Vulkan::RuntimeEngine& engine)
{

	//Model model = Model("/Models/BackPack/backpack.obj", Shader("BasicTexture", "/Shaders/"));
	Model model = Model("/Models/Asteroid/rock.obj", Shader("ColorShape", "/Shaders/"));
	//Model model = Model("/Models/Bunny/Bunny.obj", Shader("ColorShape", "/Shaders/"));
	//Model model = Model("/Models/SkyBox/SkyBox.obj", Shader("ColorShape", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };

	for (size_t i = 0; i < 145; i++)
	{
		Voronoi vorn;
		//vorn.GenerateNewPointSets(model);
		vorn.FracturePlaneRandom(model, 10, i);

		//VoronoiClipping clipper;
		//clipper.ClipMeshToVoronoi(vorn, model);
		engine.Loop();

		Vulkan::RuntimeEngine::WaitForFrameToFinish();
	}


	for (size_t i = 0; i < 145; i++)
	{
		Voronoi vorn;
		//vorn.GenerateNewPointSets(model);
		vorn.FracturePlaneRandom(model, 100, i);

		//VoronoiClipping clipper;
		//clipper.ClipMeshToVoronoi(vorn, model);
		engine.Loop();

		Vulkan::RuntimeEngine::WaitForFrameToFinish();
	}

	//for (size_t i = 0; i < 145; i++)
	//{
	//	Voronoi vorn;
	//	//vorn.GenerateNewPointSets(model);
	//	vorn.FracturePlaneRandom(model, 1000, i);

	//	VoronoiClipping clipper;
	//	clipper.ClipMeshToVoronoi(vorn, model);
	//	engine.Loop();

	//	Vulkan::RuntimeEngine::WaitForFrameToFinish();
	//}


	//while (!engine.ShouldClose())
	//{

	//}

}

void EnginePlane()
{
	Vulkan::RuntimeEngine engine;
	engine.Init();
	RunEngine(engine);

	engine.ShutDown();
}


void RunEngineDelaunay(Vulkan::RuntimeEngine& engine)
{

	Model model = Model("/Models/Asteroid/rock.obj", Shader("ColorShape", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };
	Voronoi vorn;
	vorn.FractureDelaunayRandom(model, 10);
	std::cout << "Generated Voronoi Diagram with " << vorn.Fractures.GetSize() << " cells." << std::endl;
	VoronoiClipping clipper;
	clipper.ClipMeshToVoronoi(vorn, model);

	while (!engine.ShouldClose())
	{
		engine.Loop();
	}

	Vulkan::RuntimeEngine::WaitForFrameToFinish();
}

void RunEngineOpenGL(OpenGL::RuntimeEngine engine)
{
	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	//Model model = Model("/Models/Bunny/Bunny.obj", Shader("ColorShape", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "ERROR::UNIFORMBUFFER::" << std::to_string(error) << std::endl;
	}
	Voronoi vorn;
	vorn.FracturePlaneRandom(model, 100, 0);
	//std::cout << "Generated Voronoi Diagram with " << vorn.Fractures.GetSize() << " cells." << std::endl;
	VoronoiClipping clipper;
	clipper.ClipMeshToVoronoi(vorn, model);
	while (!engine.ShouldClose())
	{
		engine.Loop();
	}
}

void OpenGLTest()
{
	OpenGL::RuntimeEngine engine;
	engine.Init();

	RunEngineOpenGL(engine);

	engine.ShutDown();
}


void EngineDelaunay()
{
	Vulkan::RuntimeEngine engine;
	engine.Init();

	RunEngineDelaunay(engine);

	engine.ShutDown();
}

TEST(Fracturing, Diagram) {
	EnginePlane();

	//EngineDelaunay();
	//OpenGLTest();
}
