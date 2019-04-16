# - Try to find Spdlog lib
#
# This sets the following variables:
# SPDLOG_FOUND - True if Cereal was found.
# SPDLOG_INCLUDE_DIRS - Directories containing the Cereal include files.


find_path(SPDLOG_INCLUDE_DIRS spdlog
	HINTS "/usr/include" "/include" "/usr/local/include")

set(SPDLOG_INCLUDE_DIRS ${SPDLOG_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(spdlog DEFAULT_MSG SPDLOG_INCLUDE_DIRS)

mark_as_advanced(SPDLOG_INCLUDE_DIRS)

if(SPDLOG_FOUND)
  MESSAGE(STATUS "Found spdlog: ${SPDLOG_INCLUDE_DIRS}")
endif(SPDLOG_FOUND)