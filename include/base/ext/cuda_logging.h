#pragma once

#include <base/logging.h>

#include <cuda.h>

#define ENSURE_CUDA(exp) \
	if (cudaError_t _rc = (exp)) \
		Base::FatalErrorLogging(Base::ErrorCodeType::CUDA, _rc, __FILE__, __LINE__, __func__, #exp).stream() \
			<< '(' << cudaGetErrorName(_rc) << " vs. " << "cudaSuccess) " << cudaGetErrorString(_rc)

#define CHECK_CUDA(exp) \
	if (cudaError_t _rc = (exp)) \
		Base::RuntimeExceptionLogging(Base::ErrorCodeType::CUDA, _rc, __FILE__, __LINE__, __func__, #exp).stream() \
			<< '(' << cudaGetErrorName(_rc) << " vs. " << "cudaSuccess) " << cudaGetErrorString(_rc)

#define LOG_IF_FAILED_CUDA(exp) \
	if (cudaError_t _rc = (exp)) \
		Base::EventLogging(__FILE__, __LINE__, __func__, #exp).stream() \
			<< '(' << cudaGetErrorName(_rc) << " vs. " << "cudaSuccess) " << cudaGetErrorString(_rc)