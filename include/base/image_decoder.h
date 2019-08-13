#pragma once
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Wincodec.h>
#include <atlbase.h>
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
		bool _globalPaletteAvaliableBit;
		uint32_t _numberOfFrames;
		uint32_t _indexOfFrame;
	};
#endif
}