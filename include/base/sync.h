#pragma once

#include <base/common.h>
typedef void * HANDLE;

namespace Base
{
	ATTRIBUTE_INTERFACE
	void waitForObject(const HANDLE waitableObject);
	ATTRIBUTE_INTERFACE
	bool waitForObject_timeout(const HANDLE waitableObject, unsigned timeout);
	ATTRIBUTE_INTERFACE
	void waitForMultipleObjects(const HANDLE* waitableObjects, unsigned numberOfObjects, unsigned *signaledObjectIndex);
	ATTRIBUTE_INTERFACE
	bool waitForMultipleObjects_timeout(const HANDLE* waitableObjects, unsigned numberOfObjects, unsigned timeout, unsigned *signaledObjectIndex);
	ATTRIBUTE_INTERFACE
	void waitAllObjects(const HANDLE* waitableObjects, unsigned numberOfObjects);
	ATTRIBUTE_INTERFACE
	bool waitAllObjects_timeout(const HANDLE* waitableObjects, unsigned numberOfObjects, unsigned timeout);
}