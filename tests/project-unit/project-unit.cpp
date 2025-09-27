#include <cassert>
#include <gtest/gtest.h>
#include "MathCore.h"
#include "Vector3D.h"
#include "Array.h"
#include "BaseDelegate.h"
#include <functional>
#include "Vector2D.h"

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

TEST(Test2, Cross)
{
	Vector3D num = Vector3D(5, 5, 5);

	Vector3D::Clamp(num, Vector3D(0, 0, 0), Vector3D(1, 1, 1));

	num.Print();

	Vector3D expected = Vector3D(0, 0, 1);

	Vector3D right = Vector3D::Cross(Vector3D(0, -1, 0), Vector3D(1, 0, 0));

	Vector3D Up = Vector3D::Cross(right, Vector3D(0, -1, 0));

	right.Print();

	Up.Print();

	bool result = Up.Z == expected.Z;
	ASSERT_EQ(Up.X, expected.Z);
}

TEST(ArrayTest1, Allocation)
{
	Array<int> test;

	test.Add(5);

	test.Add(3);

	test.Remove(0);

	test.Remove(5);

	ASSERT_EQ(3, test[0]);
}

TEST(ArrayTest2, Searching)
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

TEST(ArrayTest3, Copying)
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

TEST(DellegateTest1, Calling)
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

TEST(LinkedList1, Allocation)
{
	LinkedList<int> test;

	test.Add(5);

	test.Add(3);

	test.Remove(0);

	test.Remove(5);

	ASSERT_EQ(3, test[0]);

}

TEST(LinkedListTest2, Searching)
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

TEST(LinkedListTest3, Copying)
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

TEST(Vector2DTest1, Creation)
{
	Vector2D test;

	ASSERT_EQ(test, Vector2D::Zero);

	test = Vector2D(1, 6);

	ASSERT_EQ(test.X, 1);
	ASSERT_EQ(test.Y, 6);

}

TEST(Vector2DTest1, copying)
{
	Vector2D test = Vector2D(1, 6);

	Vector2D copy = test;

	ASSERT_EQ(test, copy);

}
