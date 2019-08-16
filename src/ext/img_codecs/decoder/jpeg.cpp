#ifdef HAVE_LIB_JPEG

#include <base/ext/img_codecs/decoder/jpeg.h>
#include <base/logging.h>

namespace Base
{
	void jpegErrorExit(j_common_ptr cinfo)
	{
		char jpegLastErrorMsg[JMSG_LENGTH_MAX];
		(*(cinfo->err->format_message)) (cinfo, jpegLastErrorMsg);

		THROW_RUNTIME_EXCEPTION << jpegLastErrorMsg;
	}
	
	JPEGDecoder::JPEGDecoder()
	{
		decInfo.err = jpeg_std_error(&jerr);
		jerr.error_exit = jpegErrorExit;
		jpeg_create_decompress(&decInfo);
	}

	JPEGDecoder::JPEGDecoder(JPEGDecoder&& object) noexcept
	{
		memcpy(&decInfo, &object.decInfo, sizeof(decInfo));
		memcpy(&jerr, &object.jerr, sizeof(jerr));
		object.decInfo.err = nullptr;
	}

	JPEGDecoder::~JPEGDecoder()
	{
		if (decInfo.err)
			jpeg_destroy_decompress(&decInfo);
	}

	void JPEGDecoder::load(const void* pointer, const uint64_t fileSize)
	{
		jpeg_mem_src(&decInfo, (const unsigned char*)pointer, fileSize);
		CHECK(jpeg_read_header(&decInfo, true));
		decInfo.out_color_space = JCS_RGB;
		decInfo.raw_data_out = false;
	}

	int JPEGDecoder::getWidth()
	{
		return decInfo.image_width;
	}

	int JPEGDecoder::getHeight()
	{
		return decInfo.image_height;
	}

	int JPEGDecoder::getDecompressedSize()
	{
		return decInfo.image_height * decInfo.image_width * decInfo.num_components;
	}

	void JPEGDecoder::decode(void* buffer)
	{
		CHECK(jpeg_start_decompress(&decInfo));

		size_t rowSize = decInfo.image_width * decInfo.num_components;
		unsigned char* currentBuffer = (unsigned char*)buffer;
		while (decInfo.output_scanline < decInfo.image_height) {
			CHECK_EQ(jpeg_read_scanlines(&decInfo, &currentBuffer, 1), 1);
			currentBuffer += rowSize;
		}
		CHECK(jpeg_finish_decompress(&decInfo));
	}
}
#endif