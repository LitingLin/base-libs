#include <base/cpu_info.h>
#include "msvc.h"

namespace Base
{
	// Initialize static member data
	const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;
	bool hasAVX()
	{
		return InstructionSet::AVX();
	}
	bool hasAVX512f()
	{
		return InstructionSet::AVX512F();
	}
}