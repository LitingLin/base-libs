#include <base/ext/img_codecs/encoder/jpeg.h>
#include <base/logging.h>

namespace Base
{

	static void jpegErrorExit(j_common_ptr cinfo)
	{
		char jpegLastErrorMsg[JMSG_LENGTH_MAX];
		(*(cinfo->err->format_message)) (cinfo, jpegLastErrorMsg);

		L_THROW_RUNTIME_EXCEPTION << jpegLastErrorMsg;
	}

	JPEGEncoder::JPEGImageContainer::JPEGImageContainer(void* ptr, size_t size)
		: EncodedImageContainer(ptr, size)
	{
	}

	JPEGEncoder::JPEGImageContainer::~JPEGImageContainer()
	{
		free(_ptr);
	}

	JPEGEncoder::JPEGEncoder()
	{
		_jpegCompress.err = jpeg_std_error(&jerr);
		jerr.error_exit = jpegErrorExit;
		jpeg_create_compress(&_jpegCompress);
		_jpegCompress.input_components = 3;
		_jpegCompress.in_color_space = JCS_RGB;
		try {
			jpeg_set_defaults(&_jpegCompress);
		}
		catch (...)
		{
			jpeg_destroy_compress(&_jpegCompress);
			throw;
		}
	}

	JPEGEncoder::~JPEGEncoder()
	{
		jpeg_destroy_compress(&_jpegCompress);
	}

	JPEGEncoder::JPEGImageContainer JPEGEncoder::encode(const void* rgb, unsigned width, unsigned height)
	{		
		_jpegCompress.image_width = width;
		_jpegCompress.image_height = height;
		unsigned char* buffer = nullptr;
		unsigned long bufferSize;
		jpeg_mem_dest(&_jpegCompress, &buffer, &bufferSize);
		L_CHECK(buffer);
		try {
			jpeg_start_compress(&_jpegCompress, TRUE);

			unsigned long rowStride = width * 3;
			JSAMPROW input[1];
			input[0] = (JSAMPROW)rgb;

			while (_jpegCompress.next_scanline < _jpegCompress.image_height)
			{
				L_CHECK_EQ(jpeg_write_scanlines(&_jpegCompress, input, 1), 1);
				input[0] += rowStride;
			}

			jpeg_finish_compress(&_jpegCompress);

			return JPEGImageContainer(buffer, bufferSize);
		}
		catch (...)
		{
			if (buffer)
				free(buffer);
			throw;
		}
	}
}
