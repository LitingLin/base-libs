#pragma once

#include <base/ext/cuda_logging.h>

class CudaStreamGuard
{
public:
	CudaStreamGuard();
	~CudaStreamGuard();
	cudaStream_t get() const;
private:
	cudaStream_t _stream;
};

class CudaMallocPitchGuard
{
public:
	CudaMallocPitchGuard(size_t width, size_t height)
	{
		CHECK_CUDA(cudaMallocPitch(&_ptr, &_pitch, width, height));
	}
	~CudaMallocPitchGuard()
	{
		LOG_IF_FAILED_CUDA(cudaFree(_ptr));
	}
	void *get()
	{
		return _ptr;
	}
	size_t getPitch()
	{
		return _pitch;
	}
private:
	size_t _pitch;
	void *_ptr;
};

class CudaMallocHostGuard
{
public:
	CudaMallocHostGuard(size_t size)
	{
		CHECK_CUDA(cudaMallocHost(&_ptr, size));
	}
	~CudaMallocHostGuard()
	{
		LOG_IF_FAILED(cudaFreeHost(_ptr));
	}
	void *get()
	{
		return _ptr;
	}
private:
	void *_ptr;
};