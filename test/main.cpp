#include "pch.h"

#include <base/logging/interface.h>
#include <base/logging/sinks/stdout.h>

int main(int argc, char* argv[]) {
	Base::Logging::addSink(new Base::Logging::StdOutSink());
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}