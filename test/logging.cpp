#include "pch.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

auto logger = spdlog::stdout_color_mt("console");