#include <base/cpu_info.h>

#ifdef __GNUC__
namespace Base
{
	bool hasAVX()
	{
		return __builtin_cpu_supports("avx") > 0;
	}

	bool hasAVX512f()
	{
		return __builtin_cpu_supports("avx512f") > 0;
	}
}
#endif