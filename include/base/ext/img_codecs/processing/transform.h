#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#endif

extern "C" {
#include <libswscale/swscale.h>
}

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined __GNUC__
#pragma GCC diagnostic pop
#endif

#include <cstdint>

namespace Base
{
	class ImageFormatTransformer
	{
	public:
		ImageFormatTransformer();
		~ImageFormatTransformer();
		void transform(uint32_t sourceImageWidth, uint32_t sourceImageHeight, AVPixelFormat sourceImageFormat,
			uint32_t sourceImageSliceX, uint32_t sourceImageSliceY, uint32_t sourceImageSliceWidth, uint32_t sourceImageSliceHeight,
			uint32_t destinationImageWidth, uint32_t destinationImageHeight, AVPixelFormat destinationImageFormat,
			uint32_t destinationImageSliceX, uint32_t destinationImageSliceY, uint32_t destinationImageSliceWidth, uint32_t destinationImageSliceHeight,
			bool doFlip,
			uint8_t* source, uint8_t* destination
			);
	private:
		SwsContext* _swsContext;
		struct Context
		{
			uint32_t sourceImageSliceWidth;
			uint32_t sourceImageSliceHeight;
			AVPixelFormat sourceImageFormat;
			uint32_t destinationImageSliceWidth;
			uint32_t destinationImageSliceHeight;
			AVPixelFormat destinationImageFormat;
		} _context;
	};

}
