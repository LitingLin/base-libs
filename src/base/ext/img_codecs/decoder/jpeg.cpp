#ifdef HAVE_LIB_JPEG

#include <base/logging.h>
#include <base/ext/img_codecs/decoder/jpeg.h>

#include <cstring>

#include <jpegint.h>

namespace Base
{
	static void jpegErrorExit(j_common_ptr cinfo)
	{
		char jpegLastErrorMsg[JMSG_LENGTH_MAX];
		(*(cinfo->err->format_message)) (cinfo, jpegLastErrorMsg);

		L_THROW_RUNTIME_EXCEPTION << jpegLastErrorMsg;
	}
	
	JPEGDecoder::JPEGDecoder()
		: _state(State::closed)
	{
		decInfo.err = jpeg_std_error(&jerr);
		jerr.error_exit = jpegErrorExit;
		jpeg_create_decompress(&decInfo);
	}

	JPEGDecoder::JPEGDecoder(JPEGDecoder&& object) noexcept
		: _state(object._state)
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
		close();
		jpeg_mem_src(&decInfo, (const unsigned char*)pointer, (unsigned long)fileSize);
		L_CHECK(jpeg_read_header(&decInfo, TRUE));
		decInfo.out_color_space = JCS_RGB;
        decInfo.raw_data_out = FALSE;
		_state = State::loaded;
	}

	unsigned JPEGDecoder::getWidth() const
	{
		L_CHECK_NE(_state, State::closed);
		return decInfo.image_width;
	}

	unsigned JPEGDecoder::getHeight() const
	{
		L_CHECK_NE(_state, State::closed);
		return decInfo.image_height;
	}

	uint64_t JPEGDecoder::getDecompressedSize() const
	{
		L_CHECK_NE(_state, State::closed);
		return decInfo.image_height * decInfo.image_width * 3;
	}

	void JPEGDecoder::decode(void* buffer)
	{
		L_CHECK_EQ(_state, State::loaded);
		L_CHECK(jpeg_start_decompress(&decInfo));
		L_CHECK_EQ(decInfo.output_components, 3);
		L_CHECK_EQ(decInfo.image_width, decInfo.output_width);
		L_CHECK_EQ(decInfo.image_height, decInfo.output_height);

		size_t rowSize = decInfo.output_width * unsigned(decInfo.output_components);
		unsigned char* currentBuffer = (unsigned char*)buffer;
		while (decInfo.output_scanline < decInfo.output_height) {
			L_CHECK_EQ(jpeg_read_scanlines(&decInfo, &currentBuffer, 1), 1);
			currentBuffer += rowSize;
		}
		L_CHECK(jpeg_finish_decompress(&decInfo));
		_state = State::decompressed;
	}

	void JPEGDecoder::close()
	{
		if (_state == State::loaded) {
			jpeg_abort_decompress(&decInfo);			
		}
		_state = State::closed;
	}
}
#endif