#include <base/ext/cuda_res_guard.h>

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
