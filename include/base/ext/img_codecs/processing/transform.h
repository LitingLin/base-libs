#pragma once

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace Base
{
	class ImageTransformer
	{
	public:
		ImageTransformer();
		~ImageTransformer();
		void initialize(uint32_t sourceImageWidth, uint32_t sourceImageHeight, AVPixelFormat sourceImageFormat, 
			uint32_t sourceImageSliceX, uint32_t sourceImageSliceY, uint32_t sourceImageSliceWidth, uint32_t sourceImageSliceHeight,
			uint32_t destinationImageWidth, uint32_t destinationImageHeight, AVPixelFormat destinationImageFormat,
			uint32_t destinationImageSliceX, uint32_t destinationImageSliceY, uint32_t destinationImageSliceWidth, uint32_t destinationImageSliceHeight,
			bool doFlip,
			uint8_t* source, uint8_t* destination
			);

		void load(void *source);
		void transform(void* destination);

	private:
		SwsContext* _context;
	};

}
