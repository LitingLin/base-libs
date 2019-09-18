#pragma once

#ifdef HAVE_LIB_WEBP

#include <base/ext/img_codecs/common.h>
#include <base/ext/img_codecs/encoder.h>
#include <cstddef>

namespace Base
{
	class IMAGE_CODECS_INTERFACE WebPEncoder
	{
	public:		
		class IMAGE_CODECS_INTERFACE WebPImageContainer : public EncodedImageContainer
		{
		public:
			WebPImageContainer(void* ptr, size_t size);
			WebPImageContainer(const WebPImageContainer&) = delete;
			WebPImageContainer(WebPImageContainer&&) = delete;
			virtual ~WebPImageContainer();
		};
		[[nodiscard]] WebPImageContainer encode(const void* rgb, unsigned width, unsigned height);
	};
}

#endif