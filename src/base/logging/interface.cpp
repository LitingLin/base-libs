#include <base/logging/interface.h>
#include <base/logging/dispatcher/dispatcher.h>

namespace Base
{
	namespace Logging
	{
		int AddSink(Sink* sink)
		{
			return Details::g_dispatcher.addSink(sink);			
		}
		void removeSink(int handle)
		{
			Details::g_dispatcher.removeSink(handle);			
		}
		void log(std::string_view message)
		{
			Details::g_dispatcher.write(message);
		}
	}
}

