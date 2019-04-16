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
	CudaMallocPitchGuard(size_t width, size_t height);
	~CudaMallocPitchGuard();
	void* get() const;
	size_t getPitch() const;
private:
	size_t _pitch;
	void *_ptr;
};

class CudaMallocHostGuard
{
public:
	CudaMallocHostGuard(size_t size);
	~CudaMallocHostGuard();
	void* get() const;
private:
	void *_ptr;
};