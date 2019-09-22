#include <base/ext/img_codecs/processing/transform.h>

#include <base/logging.h>
#include <cstring>
#include <type_traits>

#ifdef _MSC_VER
#pragma warning(push, 0)
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#endif

extern "C" {
#include <libavutil/imgutils.h>
}

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined __GNUC__
#pragma GCC diagnostic pop
#endif

namespace Base
{
	ImageFormatTransformer::ImageFormatTransformer()
		: _swsContext(nullptr)
	{
		static_assert(std::is_pod_v<Context>);
		memset(&_context, 0, sizeof(0));
	}

	ImageFormatTransformer::~ImageFormatTransformer()
	{
		sws_freeContext(_swsContext);
	}

	void ImageFormatTransformer::transform(uint32_t sourceImageWidth, uint32_t sourceImageHeight,
		AVPixelFormat sourceImageFormat, uint32_t sourceImageSliceX, uint32_t sourceImageSliceY,
		uint32_t sourceImageSliceWidth, uint32_t sourceImageSliceHeight, uint32_t destinationImageWidth,
		uint32_t destinationImageHeight, AVPixelFormat destinationImageFormat, uint32_t destinationImageSliceX,
		uint32_t destinationImageSliceY, uint32_t destinationImageSliceWidth, uint32_t destinationImageSliceHeight,
		bool flipVertical,
		uint8_t *source, uint8_t*destination)
	{
		Context context = { sourceImageSliceWidth, sourceImageSliceHeight, sourceImageFormat, destinationImageSliceWidth, destinationImageSliceHeight, destinationImageFormat };
		if (memcmp(&context, &_context, sizeof(Context)) != 0)
		{
			auto swsContext = sws_getContext(sourceImageSliceWidth, sourceImageSliceHeight, sourceImageFormat, destinationImageSliceWidth, destinationImageSliceHeight, destinationImageFormat, SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
			L_CHECK(swsContext);
			sws_freeContext(_swsContext);
			_swsContext = swsContext;
			memcpy(&_context, &context, sizeof(Context));
		}
		
		int sourceLineSize[4];
		int destinationLineSize[4];
		L_CHECK_GE(av_image_fill_linesizes(sourceLineSize, sourceImageFormat, sourceImageWidth), 0);
		L_CHECK_GE(av_image_fill_linesizes(destinationLineSize, destinationImageFormat, destinationImageWidth), 0);

		unsigned char* source_ptr[4];
		unsigned char* destination_ptr[4];

		L_CHECK_GE(av_image_fill_pointers(source_ptr, sourceImageFormat, sourceImageHeight, source, sourceLineSize), 0);
		L_CHECK_GE(av_image_fill_pointers(destination_ptr, destinationImageFormat, destinationImageHeight, destination, destinationLineSize), 0);

		// adjust X

		for (int i=0;i<4;++i)
		{
			const int stride = sourceLineSize[i];
			if (stride)
			{
				source_ptr[i] += uint64_t(stride) * uint64_t(sourceImageSliceX) / uint64_t(sourceImageWidth);
				source_ptr[i] += uint64_t(stride) * uint64_t(sourceImageSliceY);
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			const int stride = destinationLineSize[i];
			if (stride)
			{
				destination_ptr[i] += uint64_t(stride) * uint64_t(destinationImageSliceX) / uint64_t(destinationImageWidth);
				destination_ptr[i] += uint64_t(stride) * uint64_t(destinationImageSliceY);
			}
		}
		
		for (int i = 0; i < 4; ++i)
		{
			const int stride = sourceLineSize[i];
			if (stride)
			{
				source_ptr[i] += uint64_t(stride) * uint64_t(sourceImageSliceY);
			}
		}
		
		if (flipVertical)
		{
			for (int i = 0; i < 4; ++i)
			{
				const int stride = destinationLineSize[i];
				if (stride)
				{
					destination_ptr[i] += (stride * (destinationImageSliceHeight - 1));
					destinationLineSize[i] = -destinationLineSize[i];
				}
			}
		}

		L_CHECK_EQ(sws_scale(_swsContext, source_ptr, sourceLineSize, 0, sourceImageSliceHeight, destination_ptr, destinationLineSize), destinationImageSliceHeight);
	}
}
