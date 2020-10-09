#pragma once

#include <base/ext/img_codecs/common.h>

#ifdef HAVE_LIB_JPEG_TURBO
#include <turbojpeg.h>
#include <cstdint>
namespace Base {
	class IMAGE_CODECS_INTERFACE JPEGDecoder {
	public:
		JPEGDecoder();
		JPEGDecoder(const JPEGDecoder&) = delete;
		JPEGDecoder(JPEGDecoder&& object) noexcept;
		~JPEGDecoder();
		void load(const void* pointer, uint64_t size);
		int getWidth();
		int getHeight();
		int getDecompressedSize();
		void decode(void* buffer);
	private:
		const unsigned char* _pointer;
		unsigned long _fileSize;
		tjhandle _handle;
		int _width, _height;
		int _jpegSubsamp, _jpegColorspace;
	};
}
#elif defined HAVE_LIB_JPEG
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <stdio.h>
#ifdef _MSC_VER
#pragma warning(push, 0)
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#endif
#include <jpeglib.h>
#ifdef _MSC_VER
#pragma warning(pop)
#elif defined __GNUC__
#pragma GCC diagnostic pop
#endif
#include <cstdint>

namespace Base
{
	class IMAGE_CODECS_INTERFACE JPEGDecoder
	{
	public:
		JPEGDecoder();
		JPEGDecoder(const JPEGDecoder&) = delete;
		JPEGDecoder(JPEGDecoder&& object) noexcept;
		~JPEGDecoder();
		void load(const void* pointer, uint64_t size);
		[[nodiscard]] unsigned getWidth() const;
		[[nodiscard]] unsigned getHeight() const;
		[[nodiscard]] uint64_t getDecompressedSize() const;
		void decode(void* buffer);
	private:
		const unsigned char* _pointer;
		unsigned long _fileSize;

		jpeg_decompress_struct decInfo;
		struct jpeg_error_mgr jerr;
	};
}
#endif

#ifdef HAVE_INTEL_MEDIA_SDK
#include <base/memory_alignment.h>

#ifdef _MSC_VER
#pragma warning(push, 0)
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#endif
#include <mfxvideo.h> /* SDK functions in C */
#ifdef _MSC_VER
#pragma warning(pop)
#elif defined __GNUC__
#pragma GCC diagnostic pop
#endif
#include <cstdint>

#include <base/ext/img_codecs/processing/transform.h>

namespace Base {
	class IMAGE_CODECS_INTERFACE IntelGraphicsJpegDecoder
	{
	public:
		IntelGraphicsJpegDecoder();
		~IntelGraphicsJpegDecoder();
		// 32-bytes(256bits) aligned
		void load(const void* data, size_t size);

		[[nodiscard]] unsigned getWidth();
		[[nodiscard]] unsigned getHeight();
		[[nodiscard]] uint64_t getDecompressedSize();
		
		void decode(void* buffer);
	private:
		mfxSession _session;
		mfxVideoParam _param;
		Base::AlignedDynamicRawArray _buffer;
		mfxSyncPoint _syncPoint;
		int _currentHandle;
		ImageFormatTransformer _formatTransformer;
	};
}
#endif

#ifdef HAVE_NVJPEG
#include <nvjpeg.h>

namespace Base
{
	class IMAGE_CODECS_INTERFACE NVJpegDecoder
	{
	public:
		NVJpegDecoder();
		~NVJpegDecoder();
		void load(const void* data, size_t size);

		[[nodiscard]] unsigned getWidth();
		[[nodiscard]] unsigned getHeight();
		[[nodiscard]] uint64_t getDecompressedSize();

		void decode(void* buffer);
	private:
		nvjpegHandle_t _handle;
	};
}

#endif