#include <base/ext/cuda_res_raii.h>

CudaStreamGuard::CudaStreamGuard()
{
	CHECK_CUDA(cudaStreamCreate(&_stream));
}

CudaStreamGuard::~CudaStreamGuard()
{
	LOG_IF_FAILED_CUDA(cudaStreamDestroy(_stream));
}

cudaStream_t CudaStreamGuard::get() const
{
	return _stream;
}

CudaMallocPitchGuard::CudaMallocPitchGuard(size_t width, size_t height)
{
	CHECK_CUDA(cudaMallocPitch(&_ptr, &_pitch, width, height));
}

CudaMallocPitchGuard::~CudaMallocPitchGuard()
{
	LOG_IF_FAILED_CUDA(cudaFree(_ptr));
}

void* CudaMallocPitchGuard::get() const
{
	return _ptr;
}

size_t CudaMallocPitchGuard::getPitch() const
{
	return _pitch;
}

CudaMallocHostGuard::CudaMallocHostGuard(size_t size)
{
	CHECK_CUDA(cudaMallocHost(&_ptr, size));
}

CudaMallocHostGuard::~CudaMallocHostGuard()
{
	LOG_IF_FAILED(cudaFreeHost(_ptr));
}

void* CudaMallocHostGuard::get() const
{
	return _ptr;
}
