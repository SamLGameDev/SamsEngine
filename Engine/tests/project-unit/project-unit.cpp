#include <gtest/gtest.h>
#include "MathCore.h"
#include "Vector3D.h"
#include "Array.h"
#include "BaseDelegate.h"
#include "Vector2D.h"
#include "RuntimeEngine.h"
#include "Voronoi2D.h"
#include "RuntimeEngineVulkan.h"
#include "VoronoiClipping.h"
#include <string>

#include "AABB.h"
#include "FileSaving.h"
#include "PlaneClipping.h"
#include "Vector4D.h"
#include "ComputeShader/UComputeShader.h"

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



TEST(Array, EmptyConstuct)
{
	Array<int> arr;

	EXPECT_TRUE(arr.IsEmpty());
	EXPECT_EQ(arr.GetSize(), 0);
	EXPECT_EQ(arr.begin(), arr.end());
}

TEST(Array, EmptyAfterClear)
{
	Array<int> arr = { 1,2,3 };

	arr.Empty();

	EXPECT_TRUE(arr.IsEmpty());
	EXPECT_EQ(arr.GetSize(), 0);
}

TEST(Array, Add)
{
	Array<int> arr = { 1,2,3 };

	arr.Empty();

	EXPECT_TRUE(arr.IsEmpty());
	EXPECT_EQ(arr.GetSize(), 0);
}


TEST(Array, AddMultiple)
{
	Array<int> arr;

	for (int i = 0; i < 100; i++)
		arr.Add(i);

	EXPECT_EQ(arr.GetSize(), 100);

	for (int i = 0; i < 100; i++)
		EXPECT_EQ(arr[i], i);

}

TEST(Array, Insert)
{
	Array<int> arr = { 2,3 };

	arr.Insert(1, 0);

	EXPECT_EQ(arr.GetSize(), 3);
	EXPECT_EQ(arr[0], 1);
}

TEST(Array, IsertBeyondSize)
{
	Array<int> arr = { 1,2 };

	arr.Insert(3, 10);

	EXPECT_EQ(arr.GetSize(), 3);
	EXPECT_EQ(arr[2], 3);
}

TEST(Array, Remove)
{
	Array<int> arr = { 1,2,3 };

	arr.Remove(2);

	EXPECT_EQ(arr.GetSize(), 2);
	EXPECT_FALSE(arr.Contains(2));
}

TEST(Array, RemoveNonExisting)
{
	Array<int> arr = { 1,2,3 };

	arr.Remove(10);

	EXPECT_EQ(arr.GetSize(), 3);

}


TEST(Array, Replace)
{
	Array<int> arr = { 1,2,3 };

	bool result = arr.Replace(2, 10);

	EXPECT_TRUE(result);
	EXPECT_TRUE(arr.Contains(10));

}


TEST(Array, ReplaceNonExisting)
{
	Array<int> arr = { 1,2,3 };

	bool result = arr.Replace(9, 10);

	EXPECT_FALSE(result);

}


TEST(Array, ReSize)
{
	Array<int> arr = { 1,2,3 };

	bool result = arr.ReSize(10);

	EXPECT_TRUE(result);
	EXPECT_EQ(arr.GetSize(), 3);
}

TEST(Array, ResizeSmaller)
{
	Array<int> arr = { 1,2,3 };

	bool result = arr.ReSize(1);

	EXPECT_FALSE(result);
}

TEST(Array, MoveConstruct)
{
	Array<int> original = { 1,2,3 };

	Array<int> moved(std::move(original));

	EXPECT_EQ(moved.GetSize(), 3);
	EXPECT_EQ(original.GetSize(), 0);

}
TEST(Array, MoveAssignment)
{
	Array<int> a = { 1,2,3 };
	Array<int> b;

	b = std::move(a);

	EXPECT_EQ(b.GetSize(), 3);
	EXPECT_EQ(a.GetSize(), 0);

}



TEST(Array, Iterator)
{
	Array<int> arr = { 1,2,3 };

	int sum = 0;

	for (int v : arr) 
	{
		sum += v;
	}
	EXPECT_EQ(sum, 6);
}

TEST(Array, FirstAndLast)
{
	Array<int> arr = { 10,20,30 };

	EXPECT_EQ(*arr.GetFirstPtr(), 10);
	EXPECT_EQ(*arr.GetLastPtr(), 30);
}

TEST(Vector3D, NumericMax)
{
	EXPECT_EQ(Vector3D::NumericMax.X, std::numeric_limits<float>::max());
	EXPECT_EQ(Vector3D::NumericMax.Y, std::numeric_limits<float>::max());
	EXPECT_EQ(Vector3D::NumericMax.Z, std::numeric_limits<float>::max());
}

TEST(Vector3D, NumericMin)
{
	EXPECT_EQ(Vector3D::NumericMin.X, std::numeric_limits<float>::lowest());
	EXPECT_EQ(Vector3D::NumericMin.Y, std::numeric_limits<float>::lowest());
	EXPECT_EQ(Vector3D::NumericMin.Z, std::numeric_limits<float>::lowest());
}

TEST(Vector3D, Min)
{
	Vector3D a(5, 2, 8);
	Vector3D b(3, 7, 1);

	Vector3D result = Vector3D::min(a, b);

	EXPECT_FLOAT_EQ(result.X, 3);
	EXPECT_FLOAT_EQ(result.Y, 2);
	EXPECT_FLOAT_EQ(result.Z, 1);
}

TEST(Vector3D, Max)
{
	Vector3D a(5, 2, 8);
	Vector3D b(3, 7, 1);

	Vector3D result = Vector3D::max(a, b);

	EXPECT_FLOAT_EQ(result.X, 5);
	EXPECT_FLOAT_EQ(result.Y, 7);
	EXPECT_FLOAT_EQ(result.Z, 8);
}

TEST(Vector3D, SignedDistanceOnPlane)
{
	Vector3D normal(0, 1, 0);
	Vector3D pointOnPlane(0, 5, 0);
	Vector3D testPoint(2, 5, 3);

	double dist = Vector3D::GetSignedDistance(testPoint, normal, pointOnPlane);

	EXPECT_NEAR(dist, 0.0, 1e-6);
}

TEST(Vector3D, SignedDistanceAbove)
{
	Vector3D normal(0, 1, 0);
	Vector3D pointOnPlane(0, 0, 0);
	Vector3D testPoint(0, 5, 0);

	double dist = Vector3D::GetSignedDistance(testPoint, normal, pointOnPlane);

	EXPECT_GT(dist, 0);
}
TEST(Vector3D, SignedDistanceBelow)
{
	Vector3D normal(0, 1, 0);
	Vector3D pointOnPlane(0, 0, 0);
	Vector3D testPoint(0, -3, 0);

	double dist = Vector3D::GetSignedDistance(testPoint, normal, pointOnPlane);

	EXPECT_LT(dist, 0);
}

TEST(Vector3D, LineIntersection)
{
	Vector3D normal(0, 1, 0);
	double d = 0;

	Vector3D start(0, -1, 0);
	Vector3D end(0, 1, 0);

	Vector3D intersection = Vector3D::GetLineIntersectionPointWithPlane(normal, d, start, end);

	EXPECT_NEAR(intersection.X, 0, 1e-6);
	EXPECT_NEAR(intersection.Y, 0, 1e-6);
	EXPECT_NEAR(intersection.Z, 0, 1e-6);
}

TEST(Vector3D, PlaneAxis)
{
	Vector3D normal(0, 1, 0);
	Vector3D t, u;

	Vector3D::GetPlaneAxis(normal, t, u);

	EXPECT_NEAR(Vector3D::Dot(normal, t), 0, 1e-6);
	EXPECT_NEAR(Vector3D::Dot(normal, u), 0, 1e-6);
}

TEST(Vector3D, PlaneNormal)
{
	Array<Vector3D> square =
	{
		Vector3D(-1,0,-1),
		Vector3D(1,0,-1),
		Vector3D(1,0,1),
		Vector3D(-1,0,1)
	};

	Vector3D center(0, 0, 0);

	Vector3D normal = Vector3D::GetPlaneNormal(square, center);

	EXPECT_NEAR(std::abs(normal.Y), 1, 1e-6);
}

TEST(Vector3D, OrderByAngle)
{
	Array<Vector3D> points =
	{
		Vector3D(1,0,0),
		Vector3D(0,0,1),
		Vector3D(-1,0,0),
		Vector3D(0,0,-1)
	};

	Vector3D center(0, 0, 0);
	Vector3D normal(0, 1, 0);

	Vector3D::OrderByAngle(points, center, normal);

	for (size_t i = 0; i < points.GetSize(); i++)
	{
		EXPECT_TRUE(points.Contains(points[i]));
	}
}


TEST(AnglePointPair, LessThan)
{
	AnglePointPair a{ Vector3D(1,0,0),1.0 };
	AnglePointPair b{ Vector3D(0,1,0),2.0 };

	EXPECT_TRUE(a < b);
}
TEST(AnglePointPair, Equallity)
{
	AnglePointPair a{ Vector3D(1,2,3),0.1 };
	AnglePointPair b{ Vector3D(1,2,3),2.0 };

	EXPECT_TRUE(a == b);
}

TEST(Vector4D, From3D)
{
	Vector3D a = { 1, 2, 4 };

	Vector4D b = { 1, 2, 4, 5 };

	EXPECT_EQ(b, Vector4D(a, 5));
}

TEST(AABB, BoundsCorrect)
{
	Array<Vector3D> points =
	{
		Vector3D(-1, 2, 3),
		Vector3D(4, -5, 6),
		Vector3D(2, 3, -7)
	};

	FBox box(points);

	EXPECT_FLOAT_EQ(box.min.X, -1);
	EXPECT_FLOAT_EQ(box.min.Y, -5);
	EXPECT_FLOAT_EQ(box.min.Z, -7);

	EXPECT_FLOAT_EQ(box.max.X, 4);
	EXPECT_FLOAT_EQ(box.max.Y, 3);
	EXPECT_FLOAT_EQ(box.max.Z, 6);
}
TEST(AABB, FacesCorrect)
{
	FBox box;
	box.min = Vector3D(0, 0, 0);
	box.max = Vector3D(1, 1, 1);

	Array<Face> faces = box.GetFaces();

	EXPECT_EQ(faces.GetSize(), 6);

	for (const auto& face : faces)
	{
		EXPECT_EQ(face.Vertices.GetSize(), 4);
		for (const auto& v : face.Vertices)
		{
			EXPECT_TRUE(v >= box.min);
			EXPECT_TRUE(v <= box.max);
		}
	}

}

TEST(AABB, PointTests)
{
	FBox box;
	box.min = Vector3D(0, 0, 0);
	box.max = Vector3D(10, 10, 10);

	Vector3D point(5, 5, 5);

	EXPECT_TRUE(AABB::IsPointInsideBox(box, point));

	point = {15, 5, 5};
	EXPECT_FALSE(AABB::IsPointInsideBox(box, point));
	point = { 0, 5, 5 };
	EXPECT_TRUE(AABB::IsPointInsideBox(box, point));
}

TEST(AABB, BoxIntersection)
{
	FBox box1;
	box1.min = Vector3D(0, 0, 0);
	box1.max = Vector3D(5, 5, 5);

	FBox box2;
	box2.min = Vector3D(4, 4, 4);
	box2.max = Vector3D(10, 10, 10);

	EXPECT_TRUE(AABB::IsBoxIntersectingBox(box1, box2));
}
TEST(AABB, NoBoxIntersection)
{
	FBox box1;
	box1.min = Vector3D(0, 0, 0);
	box1.max = Vector3D(5, 5, 5);

	FBox box2;
	box2.min = Vector3D(6, 6, 6);
	box2.max = Vector3D(10, 10, 10);

	EXPECT_FALSE(AABB::IsBoxIntersectingBox(box1, box2));
}
TEST(AABB, BoxEdgesIntersecting)
{
	FBox box1;
	box1.min = Vector3D(0, 0, 0);
	box1.max = Vector3D(5, 5, 5);

	FBox box2;
	box2.min = Vector3D(5, 5, 5);
	box2.max = Vector3D(10, 10, 10);

	EXPECT_TRUE(AABB::IsBoxIntersectingBox(box1, box2));
}

TEST(PlaneClipping, ClipFace)
{
	Face square;
	square.Vertices = {
		Vector3D(-1,-1,0),
		Vector3D(1,-1,0),
		Vector3D(1,1,0),
		Vector3D(-1,1,0)
	};

	Vector3D planeCenter(0, 0, 0);
	Vector3D normal(1, 0, 0);

	Face outFace;
	Face intersectFace;

	PlaneClipping::ClipFaceByFace(
		square,
		planeCenter,
		outFace,
		normal,
		intersectFace
	);

	EXPECT_FALSE(outFace.Vertices.IsEmpty());
}

TEST(PlaneClipping, ClipCellByFace)
{
	Array<Face> cell;

	Face face;
	face.Vertices = {
		Vector3D(-1,-1,0),
		Vector3D(1,-1,0),
		Vector3D(1,1,0),
		Vector3D(-1,1,0)
	};

	cell.Add(face);

	Vector3D planeCenter(0, 0, 0);
	Vector3D normal(1, 0, 0);

	PlaneClipping::ClipCellByFace(cell, planeCenter, normal);

	EXPECT_FALSE(cell.IsEmpty());
}

//TEST(PlaneClipping, ClipCellByFaces)
//{
//	Array<Face> cell;
//
//	Face face;
//	face.Vertices = {
//		Vector3D(-1,-1,0),
//		Vector3D(1,-1,0),
//		Vector3D(1,1,0),
//		Vector3D(-1,1,0)
//	};
//
//	cell.Add(face);
//
//	Array<Face> clippingPlanes;
//	clippingPlanes.Add(face);
//
//	PlaneClipping::ClipCellByFaces(cell, clippingPlanes);
//
//	EXPECT_FALSE(cell.IsEmpty());
//}

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

	for (size_t i = 0; i < 1; i++)
	{
		Voronoi vorn;
		//vorn.GenerateNewPointSets(model);
		vorn.FracturePlaneRandom(model, 10, i);

		//VoronoiClipping clipper;
		//clipper.ClipMeshToVoronoi(vorn, model);
		//engine.Loop();

		//Vulkan::RuntimeEngine::WaitForFrameToFinish();
	}


	//for (size_t i = 0; i < 145; i++)
	//{
	//	Voronoi vorn;
	//	//vorn.GenerateNewPointSets(model);
	//	vorn.FracturePlaneRandom(model, 100, i);

	//	//VoronoiClipping clipper;
	//	//clipper.ClipMeshToVoronoi(vorn, model);
	//	engine.Loop();

	//	Vulkan::RuntimeEngine::WaitForFrameToFinish();
	//}

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


	while (!engine.ShouldClose())
	{
		engine.Loop();
	}

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
	vorn.FracturePlaneRandomGPU(model, 10, 0);
	//std::cout << "Generated Voronoi Diagram with " << vorn.Fractures.GetSize() << " cells." << std::endl;
	//VoronoiClipping clipper;
	//clipper.ClipMeshToVoronoi(vorn, model);

	Vulkan::RuntimeEngine::WaitForFrameToFinish();
}

void RunEngineOpenGL(OpenGL::RuntimeEngine engine)
{
	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };
	Voronoi vorn;
	vorn.FracturePositions.Position = { 0, 0, 0 };
	vorn.FracturePlaneRandomGPU(model, 10, 0, true);

	Voronoi vorn100;
	vorn100.FracturePositions.Position = { 10, 0 ,0 };
	vorn100.FracturePlaneRandomGPU(model, 100, 0, true);

	while (!engine.ShouldClose())
	{
		engine.Loop();
	}

	Vulkan::RuntimeEngine::WaitForFrameToFinish();
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
	//EnginePlane();

	//EngineDelaunay();
	OpenGLTest();
}
