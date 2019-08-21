#pragma once

#include <base/common.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Base {
	class ATTRIBUTE_INTERFACE AllowAllSecurityATTRIBUTES
	{
	public:
		AllowAllSecurityATTRIBUTES();
		~AllowAllSecurityATTRIBUTES();
		SECURITY_ATTRIBUTES *get();
	private:
		PSECURITY_DESCRIPTOR _descriptor;
		SECURITY_ATTRIBUTES _attributes;
	};
}