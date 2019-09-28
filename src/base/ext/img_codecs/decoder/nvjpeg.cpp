#ifdef HAVE_NVJPEG
#include <base/ext/img_codecs/decoder/jpeg.h>

#include <base/logging.h>

namespace Base
{
	NVJpegDecoder::NVJpegDecoder()
	{
		L_CHECK_EQ(nvjpegCreateSimple(&_handle), NVJPEG_STATUS_SUCCESS);
	}

	NVJpegDecoder::~NVJpegDecoder()
	{
		L_LOG_IF_NOT_EQ(nvjpegDestroy(_handle), NVJPEG_STATUS_SUCCESS);
	}

	void NVJpegDecoder::load(const void* data, size_t size)
	{
		int widths[NVJPEG_MAX_COMPONENT];
		int heights[NVJPEG_MAX_COMPONENT];
		int n_components;
		nvjpegChromaSubsampling_t chroma_subsampling;
		L_CHECK_EQ(nvjpegGetImageInfo(_handle, (const unsigned char*)data, size, &n_components, &chroma_subsampling, widths, heights), NVJPEG_STATUS_SUCCESS);
	}
}
#endif