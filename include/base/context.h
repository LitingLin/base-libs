#pragma once
#include <vector>
#include <mutex>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Rpc.h>
#include <map>

bool operator<(const GUID& left, const GUID& right);

class ConcurrentObjectInstanceMapper
{
public:
	std::vector<std::pair<GUID, std::vector<GUID>>> get();
	std::vector<GUID> getObjectList();
	std::vector<GUID> getInstanceList();
	std::vector<GUID> getInstanceList(GUID object);
	bool getObject(GUID instance, GUID *object);
	bool isObjectExists(GUID object);
	bool newObject(GUID object);
	bool newInstance(GUID object, GUID instance);
private:
	std::map<GUID, std::vector<GUID>> _forwardMapper; // object to instance
	std::map<GUID, GUID> _reverseMapper; // instance to object
	std::mutex _lock;
};
