#include <spdlog/spdlog.h>

#ifdef _WIN32
#include <base/debugoutput_logger_sink.h>
auto logger = std::make_shared<spdlog::logger>("console", std::make_shared<Base::debugoutput_sink>());
#else
#include <spdlog/sinks/stdout_sinks.h>
auto logger = spdlog::stdout_color_mt("console");
#endif