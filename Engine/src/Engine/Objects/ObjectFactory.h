// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once

#include "Object.h"


template<typename T>
concept obj = (std::is_base_of_v<Object, T>);

/**
 * Helper class for creating anything derived from object.
 */
class ObjectFactory
{
};


/**
 * Creates a pointer to an object, allocated using new, so make sure to delete, or assign to a smart ptr
 * @tparam T The Type of object to create
 * @param args Any args the constructor of the object takes
 */
template<typename T, typename... Args>
T* CreateObjectPtr(Args... args) requires obj<T>
{
	T* object = new T(args...);
	object->Start();
	return object;
}

/**
 *  Creates an object of the specified type, allocated on the stack, so no need for management
 * @tparam T The Type of object to create
 * @param args Any args the constructor of the object takes
 */
template<typename T, typename... Args>
T CreateObjectRaw(Args... args) requires obj<T>
{
	T object = T(args...);
	object.Start();
	return object;
}
