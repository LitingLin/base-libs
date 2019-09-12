#ifdef HAVE_LIB_PNG
#include <base/ext/img_codecs/decoder/png.h>
#include <base/logging.h>
#include <cstring>

namespace Base
{
	void png_error(png_structp png_ptr, png_const_charp message)
	{
		(png_ptr);
		L_THROW_RUNTIME_EXCEPTION << message;
	}
	void png_warning(png_structp png_ptr, png_const_charp message)
	{
		(png_ptr);
		L_LOG_ERROR << message;
	}

	PNGDecoder::PNGDecoder()
		: _png_ptr(nullptr), _info_ptr(nullptr)
	{
	}

	PNGDecoder::PNGDecoder(PNGDecoder&& object) noexcept
	{
		_sourceImage = object._sourceImage;
		_currentImagePosition = object._currentImagePosition;
		_png_ptr = object._png_ptr;
		_info_ptr = object._info_ptr;
		_image_width = object._image_width;
		_image_height = object._image_height;
		_row_pointers = std::move(object._row_pointers);
		object._png_ptr = nullptr;
	}

	PNGDecoder::~PNGDecoder()
	{
		if (_png_ptr)
			png_destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)NULL);
	}

	unsigned PNGDecoder::getWidth() const
	{
		return _image_width;
	}

	unsigned PNGDecoder::getHeight() const
	{
		return _image_height;
	}

	uint64_t PNGDecoder::getDecompressedSize()
	{
		return uint64_t(_image_width) * uint64_t(_image_height) * 3ULL;
	}

	void PNGDecoder::decode(void* buffer)
	{
		if (_row_pointers.size() != _image_height)
			_row_pointers.resize(_image_height);
		for (unsigned long i = 0; i < _image_height; i++) {
			_row_pointers[i] = (png_bytep)((char*)buffer + i * _image_width * 3);
		}

		png_read_image(_png_ptr, _row_pointers.data());
		_currentImagePosition = _sourceImage;
	}

	void readDataCallback(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		unsigned char** source_data = (unsigned char**)png_get_io_ptr(png_ptr);
		memcpy(data, *source_data, length);

		*source_data += length;
	}

	void PNGDecoder::load(const void* image, uint64_t size)
	{
		if (_png_ptr)
			png_destroy_read_struct(&_png_ptr, &_info_ptr, nullptr);

		_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		L_CHECK(_png_ptr) << "png_create_read_struct()";
		_info_ptr = png_create_info_struct(_png_ptr);
		L_CHECK_WITH_FINALIZER(_info_ptr, [&]() {png_destroy_read_struct(&_png_ptr, nullptr, nullptr); _png_ptr = nullptr; }) << "png_create_info_struct()";
		_sourceImage = (unsigned char*)image;

		L_CHECK_GE(size, 8) << "Invalid image";

		L_CHECK_EQ(png_sig_cmp(_sourceImage, 0, 8), 0) << "Invalid image";
		_currentImagePosition = _sourceImage;
		png_set_read_fn(_png_ptr, &_currentImagePosition, readDataCallback);

		png_set_sig_bytes(_png_ptr, 0);

		png_read_info(_png_ptr, _info_ptr);
		_image_width = png_get_image_width(_png_ptr, _info_ptr);
		_image_height = png_get_image_height(_png_ptr, _info_ptr);
		unsigned char bit_depth = png_get_bit_depth(_png_ptr, _info_ptr);
		unsigned char color_type = png_get_color_type(_png_ptr, _info_ptr);

		if (color_type & PNG_COLOR_MASK_ALPHA)
			png_set_strip_alpha(_png_ptr);

		// force palette images to be expanded to 24-bit RGB  
		// it may include alpha channel  
		if (color_type == PNG_COLOR_TYPE_PALETTE) {
			png_set_palette_to_rgb(_png_ptr);
		}

		// low-bit-depth grayscale images are to be expanded to 8 bits  
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
			png_set_expand_gray_1_2_4_to_8(_png_ptr);
		}

		// reduce images with 16-bit samples to 8 bits
		if (bit_depth == 16) {
			png_set_strip_16(_png_ptr);
		}

		// expand grayscale images to RGB  
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
			png_set_gray_to_rgb(_png_ptr);
		}

		png_set_interlace_handling(_png_ptr);
		png_read_update_info(_png_ptr, _info_ptr);
	}
}
#endif