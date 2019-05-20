#pragma once
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Wincodec.h>
#include <atlbase.h>
#endif

#ifdef HAVE_LIB_JPEG_TURBO
#include <turbojpeg.h>
#endif

#ifdef HAVE_LIB_PNG
#include <png.h>
#include <string>
#include <vector>
#endif

namespace Base
{
#ifdef _WIN32
	class ImageDecoder
	{
	public:
		ImageDecoder(const unsigned char *data, uint32_t data_size);
		// index auto increment 
		std::vector<unsigned char> decode();
		// index auto increment 
		void decode(unsigned char *buf, uint32_t buf_size);
		void decodeAndScale(unsigned char *buf, uint32_t width, uint32_t height, uint32_t buf_size);
		void getResolution(uint32_t *width, uint32_t *height);
		void getDPI(double *dpix, double *dpiy);
		uint32_t getBufferSize();
		uint32_t getNumberOfFrames();
		GUID getFormat();
		void setIndexOfFrame(uint32_t index);
		uint32_t getDelay();
	private:
		CComPtr<IWICImagingFactory> _WICFactory;
		CComPtr<IWICStream> _WICStream;
		CComPtr<IWICBitmapDecoder> _WICBitmapDecoder;
		CComPtr<IWICPalette> _WICPalette;
		CComPtr<IWICFormatConverter> _WICFormatConverter;
		CComPtr<IWICStream> _IWICStream;
		CComPtr<IWICBitmapFrameDecode> _WICBitmapFrameDecoder;
		GUID _formatType;
		bool _globalPaletteAvaliableBit;
		uint32_t _numberOfFrames;
		uint32_t _indexOfFrame;
	};
#endif

#ifdef HAVE_LIB_JPEG_TURBO
	class JPEGDecoder
	{
	public:
		JPEGDecoder();
		JPEGDecoder(const JPEGDecoder&) = delete;
		JPEGDecoder(JPEGDecoder&& object) noexcept;
		~JPEGDecoder();
		void load(const void* pointer, const uint64_t size);
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
#endif

#ifdef HAVE_LIB_PNG
	class PNGDecoder
	{
	public:
		PNGDecoder();
		PNGDecoder(const PNGDecoder&) = delete;
		PNGDecoder(PNGDecoder&& object) noexcept;
		~PNGDecoder();
		void load(const void* image, uint64_t size);
		int getWidth() const;
		int getHeight() const;
		int getDecompressedSize();
		void decode(void* buffer);
	private:
		unsigned char* _sourceImage;
		unsigned char* _currentImagePosition;
		png_structp _png_ptr;
		png_infop _info_ptr;
		unsigned long _image_width, _image_height;
		::std::vector<png_bytep> _row_pointers;
	};
#endif

}