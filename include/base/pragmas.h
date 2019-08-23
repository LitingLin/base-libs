#pragma once

#ifdef _MSC_VER
#define SUPPRESS_ALL_WARNING_BEGIN \
#pragma warning(push, 0)
#define SUPPRESS_ALL_WARNING_END \
#pragma warning(pop)
#elif defined __GNUC__
#define SUPPRESS_ALL_WARNING_BEGIN \
#pragma GCC diagnostic push \
#pragma GCC diagnostic ignored "-Wall"
#define SUPPRESS_ALL_WARNING_END \
#pragma GCC diagnostic pop
#endif
