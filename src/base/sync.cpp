#include <base/sync.h>

#define NOMINMAX
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <base/logging.h>

namespace Base
{
	void waitForObject(const HANDLE waitableObject)
	{
		unsigned rc = WaitForSingleObject(waitableObject, INFINITE);
		L_ENSURE_EQ_WIN32API(rc, WAIT_OBJECT_0);
	}

	bool waitForObject_timeout(const HANDLE waitableObject, unsigned timeout)
	{
		unsigned rc = WaitForSingleObject(waitableObject, timeout);
		if (rc == WAIT_TIMEOUT)
			return false;
		L_ENSURE_EQ_WIN32API(rc, WAIT_OBJECT_0);
		return true;
	}

	void waitForMultipleObjects(const HANDLE* waitableObjects, unsigned numberOfObjects, unsigned *signaledObjectIndex)
	{
		L_ENSURE_LE(numberOfObjects, unsigned(MAXIMUM_WAIT_OBJECTS));

		unsigned rc = WaitForMultipleObjects(numberOfObjects, waitableObjects, FALSE, INFINITE);
		L_ENSURE_GE_WIN32API(rc, WAIT_OBJECT_0);
		L_ENSURE_LT_WIN32API(rc, WAIT_OBJECT_0 + numberOfObjects);
		*signaledObjectIndex = rc - WAIT_OBJECT_0;
	}

	bool waitForMultipleObjects_timeout(const HANDLE* waitableObjects, unsigned numberOfObjects, unsigned timeout, unsigned *signaledObjectIndex)
	{
		L_ENSURE_LE(numberOfObjects, unsigned(MAXIMUM_WAIT_OBJECTS));

		unsigned rc = WaitForMultipleObjects(numberOfObjects, waitableObjects, FALSE, timeout);
		if (rc == WAIT_TIMEOUT)
			return false;
		L_ENSURE_GE_WIN32API(rc, WAIT_OBJECT_0);
		L_ENSURE_LT_WIN32API(rc, WAIT_OBJECT_0 + numberOfObjects);
		*signaledObjectIndex = rc - WAIT_OBJECT_0;
		return true;
	}

	void waitAllObjects(const HANDLE* waitableObjects, unsigned numberOfObjects)
	{
		L_ENSURE_LE(numberOfObjects, unsigned(MAXIMUM_WAIT_OBJECTS));

		unsigned rc = WaitForMultipleObjects(numberOfObjects, waitableObjects, TRUE, INFINITE);
		L_ENSURE_GE_WIN32API(rc, WAIT_OBJECT_0);
		L_ENSURE_LT_WIN32API(rc, WAIT_OBJECT_0 + numberOfObjects);
	}

	bool waitAllObjects_timeout(const HANDLE* waitableObjects, unsigned numberOfObjects, unsigned timeout)
	{
		L_ENSURE_LE(numberOfObjects, unsigned(MAXIMUM_WAIT_OBJECTS));

		unsigned rc = WaitForMultipleObjects(numberOfObjects, waitableObjects, TRUE, timeout);
		if (rc == WAIT_TIMEOUT)
			return false;
		L_ENSURE_GE_WIN32API(rc, WAIT_OBJECT_0);
		L_ENSURE_LT_WIN32API(rc, WAIT_OBJECT_0 + numberOfObjects);
		return true;
	}
}