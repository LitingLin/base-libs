#include "pch.h"

#include <base/logging/interface.h>
#include <base/logging/sinks/stdout.h>

class LoggerInitializer
{
public:
	LoggerInitializer()
	{
		Base::Logging::addSink(new Base::Logging::StdOutSink());
	}
} initializer;