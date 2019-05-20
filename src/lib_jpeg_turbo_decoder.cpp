#ifdef HAVE_LIB_JPEG_TURBO

#include <base/image_decoder.h>
#include <base/logging.h>

namespace Base
{
	JPEGDecoder::JPEGDecoder()
	{
		_handle = tjInitDecompress();
		CHECK(_handle) << tjGetErrorStr2(nullptr) << "tjInitDecompress() failed.";
	}

	JPEGDecoder::JPEGDecoder(JPEGDecoder&& object) noexcept
	{
		this->_handle = object._handle;
		object._handle = nullptr;
	}

	JPEGDecoder::~JPEGDecoder()
	{
		if (_handle)
			tjDestroy(_handle);
	}

	void JPEGDecoder::load(const void* pointer, const uint64_t fileSize)
	{
		CHECK_LE(fileSize, std::numeric_limits<unsigned long>::max()) << "Cannot process images which file size larger than " << std::numeric_limits<unsigned long>::max() << " bytes.";

		const auto fileSize_safeCast = static_cast<const unsigned long>(fileSize);

		CHECK_NE(tjDecompressHeader3(_handle, (const unsigned char*)pointer, fileSize_safeCast, &_width, &_height, &_jpegSubsamp, &_jpegColorspace), -1) << tjGetErrorStr2(_handle);
		_pointer = (const unsigned char*)pointer;
		_fileSize = fileSize_safeCast;
	}

	int JPEGDecoder::getWidth()
	{
		return _width;
	}

	int JPEGDecoder::getHeight()
	{
		return _height;
	}

	int JPEGDecoder::getDecompressedSize()
	{
		return _width * _height* tjPixelSize[TJPF_RGB];
	}

	void JPEGDecoder::decode(void* buffer)
	{
		CHECK_NE(tjDecompress2(_handle, _pointer, _fileSize, (unsigned char*)buffer, _width, _width * tjPixelSize[TJPF_RGB], _height, TJPF_RGB, TJFLAG_ACCURATEDCT), -1) << tjGetErrorStr2(_handle);
	}
}
#endif