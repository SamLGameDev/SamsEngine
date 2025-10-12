#include <cassert>
#include <gtest/gtest.h>
#include "MathCore.h"
#include "Vector3D.h"
#include "Array.h"
#include "BaseDelegate.h"
#include <functional>
#include "Vector2D.h"
#include "CorePaths.h"
#include "RuntimeEngine.h"
#include "Voronoi2D.h"

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

TEST(Array, Copying)
{
	Array<int> test;

	test.Add(5);

	test.Add(3);

	Array<int> copy = test;

	bool btest = copy == test;

	ASSERT_EQ(btest, true);

	copy.Remove(3);
	btest = copy == test;

	ASSERT_EQ(btest, false);

}

TEST(Dellegate, Calling)
{
	MulticastDelegate<bool&> TestDel;

	bool flag = false;

	std::function<void(bool&)> TestFunc = [](bool& flag)
	{
			flag = true;
	};

	TestDel.BindMember(TestFunc);

	TestDel.Broadcast(flag);

	ASSERT_EQ(true, flag);

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

TEST(FileSystem, Contents)
{
	CorePaths pathManager = CorePaths();

	ASSERT_EQ(pathManager.Contents.Path, CONTENTS_DIR);
}

TEST(Vector2D, PerpendicularBisector)
{
	RuntimeEngine engine;
	engine.Init();

	Voronoi2D v;
	v.FracturePlaneRandom(Vector2D(-1, 1), Vector2D(-1, -1), Vector2D(1, 1), Vector2D(1, -1));

	while (!RuntimeEngine::ShouldClose())
	{
		engine.Loop();
	}

	engine.ShutDown();
}
