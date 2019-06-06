#ifdef _MSC_VER
#include <base/cpu_info/msvc.h>

namespace Base
{
	bool hasAVX()
	{
		return InstructionSet::AVX();
	}
	bool hasAVX512f()
	{
		return InstructionSet::AVX512F();
	}
}

#endif