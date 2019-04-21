#pragma once
// General utility macro
#define _PP_CAT( A, B ) A ## B
#define _PP_EXPAND(...) __VA_ARGS__

// Macro overloading feature support
#define _PP_VA_ARG_SIZE(...) _PP_EXPAND(_PP_APPLY_ARG_N((_PP_ZERO_ARGS_DETECT(__VA_ARGS__), _PP_RSEQ_N)))

#define _PP_ZERO_ARGS_DETECT(...) _PP_EXPAND(_PP_ZERO_ARGS_DETECT_PREFIX_ ## __VA_ARGS__ ## _ZERO_ARGS_DETECT_SUFFIX)
#define _PP_ZERO_ARGS_DETECT_PREFIX__ZERO_ARGS_DETECT_SUFFIX ,,,,,,,,,,,0

#define _PP_APPLY_ARG_N(ARGS) _PP_EXPAND(_PP_ARG_N ARGS)
#define _PP_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N,...) N
#define _PP_RSEQ_N 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define _PP_OVERLOAD_SELECT(NAME, NUM) _PP_CAT( NAME ## _, NUM)
#define _PP_MACRO_OVERLOAD(NAME, ...) _PP_EXPAND(_PP_OVERLOAD_SELECT(NAME, _PP_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__))