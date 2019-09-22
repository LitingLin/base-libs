#include <base/ext/img_codecs/processing/transform.h>

#include <base/logging.h>

namespace Base
{
	ImageTransformer::ImageTransformer()
		: _context(nullptr)
	{
	}

	void ImageTransformer::initialize(uint32_t sourceImageWidth, uint32_t sourceImageHeight,
		AVPixelFormat sourceImageFormat, uint32_t sourceImageSliceX, uint32_t sourceImageSliceY,
		uint32_t sourceImageSliceWidth, uint32_t sourceImageSliceHeight, uint32_t destinationImageWidth,
		uint32_t destinationImageHeight, AVPixelFormat destinationImageFormat, uint32_t destinationImageSliceX,
		uint32_t destinationImageSliceY, uint32_t destinationImageSliceWidth, uint32_t destinationImageSliceHeight,
		bool flipVertical,
		uint8_t *source, uint8_t*destination)
	{
		_context = sws_getContext(sourceImageSliceWidth, sourceImageSliceHeight, sourceImageFormat, destinationImageSliceWidth, destinationImageSliceHeight, destinationImageFormat, SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
		L_CHECK(_context);

		int sourceLineSize[4];
		int destinationLineSize[4];
		L_CHECK_GE(av_image_fill_linesizes(sourceLineSize, sourceImageFormat, sourceImageWidth), 0);
		L_CHECK_GE(av_image_fill_linesizes(destinationLineSize, destinationImageFormat, destinationImageWidth), 0);

		unsigned char* source_ptr[4];
		unsigned char* destination_ptr[4];

		av_image_fill_pointers(source_ptr, sourceImageFormat, sourceImageHeight, source, sourceLineSize);
		av_image_fill_pointers(destination_ptr, destinationImageFormat, destinationImageHeight, destination, destinationLineSize);


		
		if (flipVertical)
		{
			for (int i = 0; i < 4; ++i)
			{
				const int stride = destinationLineSize[i];
				if (stride)
				{
					destination_ptr[i] += (stride * (_destination_height - 1));
					destination_linesize[i] = -destination_linesize[i];
				}
			}
		}
		sws_scale(_context, source_ptr, sourceLineSize, sourceImageSliceY, sourceImageSliceHeight, destination_ptr, destinationLineSize);
		
		sws_freeContext(_context);
	}

	void ImageTransformer::load(void* source)
	{
	}
}
