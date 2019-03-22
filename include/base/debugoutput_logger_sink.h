#pragma once

#include <spdlog/sinks/base_sink.h>

namespace Base {
	class debugoutput_sink : public spdlog::sinks::base_sink<std::mutex>
	{
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;
		void flush_() override;
	};
}