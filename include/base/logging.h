#pragma once

#include <base/logging/base.h>

#include <base/exception.h>
#include <base/error_codes/stringify.h>

#include <base/preprocessor.h>
#include <base/logging/macros.h>
#include <base/logging/interface.h>

#ifdef _WIN32
#include <base/logging/win32.h>
#endif