#ifdef _WIN32
#include <base/image_decoder.h>
#include <base/logging.h>

#include <dshow.h>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "Strmiids.lib")

namespace Base {
	std::vector<unsigned char> WindowsImageDecoder::decode()
	{
		L_CHECK_LT(_indexOfFrame, _numberOfFrames);
		if (!_WICBitmapFrameDecoder) {
			L_CHECK_HR(_WICBitmapDecoder->GetFrame(_indexOfFrame, &_WICBitmapFrameDecoder));
		}
		UINT width, height;
		L_CHECK_HR(_WICBitmapFrameDecoder->GetSize(&width, &height));
		std::vector<unsigned char> buf;
		size_t buf_size = width * height * 3;
		L_CHECK_LE(buf_size, std::numeric_limits<UINT>::max());
		buf.resize(buf_size);
		WICPixelFormatGUID format;
		L_CHECK_HR(_WICBitmapFrameDecoder->GetPixelFormat(&format));
		if (format != GUID_WICPixelFormat24bppBGR)
		{
			if (_globalPaletteAvaliableBit)
			{
				WICBitmapPaletteType paletteType;
				L_CHECK_HR(_WICPalette->GetType(&paletteType));
				_WICFormatConverter->Initialize(_WICBitmapFrameDecoder, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, _WICPalette, 0.0f, paletteType);
			}
			else
			{
				HRESULT hr = _WICBitmapFrameDecoder->CopyPalette(_WICPalette);
				if (FAILED(hr))
				{
					L_CHECK_EQ_HR(hr, WINCODEC_ERR_PALETTEUNAVAILABLE);
					WICBitmapPaletteType paletteType;
					L_CHECK_HR(_WICPalette->GetType(&paletteType));
					_WICFormatConverter->Initialize(_WICBitmapFrameDecoder, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, _WICPalette, 0.0f, paletteType);
				}
				else
				{
					_WICFormatConverter->Initialize(_WICBitmapFrameDecoder, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
				}
			}
			L_CHECK_HR(_WICFormatConverter->CopyPixels(nullptr, width * 3, (UINT)buf_size, buf.data()));
		}
		else
			_WICBitmapFrameDecoder->CopyPixels(nullptr, width * 3, (UINT)buf_size, buf.data());

		return buf;
	}

	WICBitmapTransformOptions getTransformOptions(IWICBitmapDecoder *decoder, IWICBitmapFrameDecode *frameDecoder)
	{
		WICBitmapTransformOptions TransformOptions = WICBitmapTransformRotate0;
		CComPtr<IWICMetadataQueryReader> WICMetadataQueryReader;
		if (SUCCEEDED(decoder->GetMetadataQueryReader(&WICMetadataQueryReader)) || SUCCEEDED(frameDecoder->GetMetadataQueryReader(&WICMetadataQueryReader)))
		{
			PROPVARIANT variant;
			::PropVariantInit(&variant);
			if (SUCCEEDED(WICMetadataQueryReader->GetMetadataByName(L"System.Photo.Orientation", &variant)))
			{
				if (variant.vt == VT_UI2)
				{
					switch (variant.uiVal)
					{
					default:
					case 1:
						TransformOptions = WICBitmapTransformRotate0;
						break;
					case 2:
						TransformOptions = WICBitmapTransformFlipHorizontal;
						break;
					case 3:
						TransformOptions = WICBitmapTransformRotate180;
						break;
					case 4:
						TransformOptions = WICBitmapTransformFlipVertical;
						break;
					case 6:
						TransformOptions = WICBitmapTransformRotate90;
						break;
					case 8:
						TransformOptions = WICBitmapTransformRotate270;
						break;
					}
				}
			}
			::PropVariantClear(&variant);
		}

		return TransformOptions;
	}

	void WindowsImageDecoder::decode(unsigned char* buf, uint32_t buf_size)
	{
		L_CHECK_LT(_indexOfFrame, _numberOfFrames);
		if (!_WICBitmapFrameDecoder) {
			L_CHECK_HR(_WICBitmapDecoder->GetFrame(_indexOfFrame, &_WICBitmapFrameDecoder));
		}
		UINT width, height;
		L_CHECK_HR(_WICBitmapFrameDecoder->GetSize(&width, &height));
		WICPixelFormatGUID format;
		L_CHECK_HR(_WICBitmapFrameDecoder->GetPixelFormat(&format));
		if (format != GUID_WICPixelFormat24bppBGR)
		{
			if (_globalPaletteAvaliableBit)
			{
				WICBitmapPaletteType paletteType;
				L_CHECK_HR(_WICPalette->GetType(&paletteType));
				_WICFormatConverter->Initialize(_WICBitmapFrameDecoder, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, _WICPalette, 0.0f, paletteType);
			}
			else
			{
				HRESULT hr = _WICBitmapFrameDecoder->CopyPalette(_WICPalette);
				if (FAILED(hr))
				{
					L_CHECK_EQ_HR(hr, WINCODEC_ERR_PALETTEUNAVAILABLE);
					WICBitmapPaletteType paletteType;
					L_CHECK_HR(_WICPalette->GetType(&paletteType));
					_WICFormatConverter->Initialize(_WICBitmapFrameDecoder, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, _WICPalette, 0.0f, paletteType);
				}
				else
				{
					_WICFormatConverter->Initialize(_WICBitmapFrameDecoder, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
				}
			}
			L_CHECK_HR(_WICFormatConverter->CopyPixels(nullptr, width * 3, buf_size, buf));
		}
		else
		{
			_WICBitmapFrameDecoder->CopyPixels(nullptr, width * 3, buf_size, buf);
			++_indexOfFrame;
			_WICBitmapFrameDecoder.Release();
		}
	}

	void determineSize(uint32_t old_width, uint32_t old_height, uint32_t width, uint32_t height, uint32_t *new_width, uint32_t *new_height)
	{
		double width_ratio = double(width) / double(old_width);
		double height_ratio = double(height) / double(old_height);
		double ratio = (width_ratio < height_ratio) ? width_ratio : height_ratio;
		*new_width = uint32_t(old_width * ratio);
		*new_height = uint32_t(old_height *ratio);
	}

	void rotate(unsigned char *src, unsigned char *dst, uint32_t srcWidth, uint32_t srcHeight, WICBitmapTransformOptions transformOption,
		uint32_t *dstWidth, uint32_t *dstHeight,
		IWICImagingFactory *factory)
	{
		CComPtr<IWICBitmap> srcBitmap;
		L_CHECK_HR(factory->CreateBitmapFromMemory(srcWidth, srcHeight, GUID_WICPixelFormat24bppBGR, srcWidth * 3, srcWidth*srcHeight * 3, src, &srcBitmap));

		CComPtr<IWICBitmapFlipRotator> WICBitmapFlipRotator;
		L_ENSURE_HR(factory->CreateBitmapFlipRotator(&WICBitmapFlipRotator));
		L_CHECK_HR(WICBitmapFlipRotator->Initialize(srcBitmap, transformOption));
		L_CHECK_HR(WICBitmapFlipRotator->GetSize(dstWidth, dstHeight));

		L_CHECK_HR(WICBitmapFlipRotator->CopyPixels(nullptr, *dstWidth * 3, srcWidth*srcHeight * 3, dst));
	}

	void WindowsImageDecoder::decodeAndScale(unsigned char* buf, uint32_t maxWidth, uint32_t maxHeight, uint32_t buf_size)
	{
		L_CHECK_LT(_indexOfFrame, _numberOfFrames);
		std::vector<unsigned char> buffer;
		CComPtr<IWICBitmapSource> bitmapSource;

		if (!_WICBitmapFrameDecoder) {
			L_CHECK_HR(_WICBitmapDecoder->GetFrame(_indexOfFrame, &_WICBitmapFrameDecoder));
		}
		bitmapSource = _WICBitmapFrameDecoder;

		WICPixelFormatGUID format;
		L_CHECK_HR(bitmapSource->GetPixelFormat(&format));
		if (format != GUID_WICPixelFormat24bppBGR)
		{
			if (_globalPaletteAvaliableBit)
			{
				WICBitmapPaletteType paletteType;
				L_CHECK_HR(_WICPalette->GetType(&paletteType));
				L_CHECK_HR(_WICFormatConverter->Initialize(bitmapSource, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, _WICPalette, 0.0f, paletteType));
			}
			else
			{
				HRESULT hr = _WICBitmapFrameDecoder->CopyPalette(_WICPalette);
				if (FAILED(hr))
				{
					L_CHECK_EQ_HR(hr, WINCODEC_ERR_PALETTEUNAVAILABLE);
					L_CHECK_HR(_WICFormatConverter->Initialize(bitmapSource, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom));
				}
				else
				{
					WICBitmapPaletteType paletteType;
					L_CHECK_HR(_WICPalette->GetType(&paletteType));
					L_CHECK_HR(_WICFormatConverter->Initialize(bitmapSource, GUID_WICPixelFormat24bppBGR, WICBitmapDitherTypeNone, _WICPalette, 0.0f, paletteType));
				}
			}
			bitmapSource = _WICFormatConverter;
		}
		UINT imageWidth, imageHeight;
		L_CHECK_HR(bitmapSource->GetSize(&imageWidth, &imageHeight));

		WICBitmapTransformOptions transformOptions = getTransformOptions(_WICBitmapDecoder, _WICBitmapFrameDecoder);
		uint32_t scaledWidth, scaledHeight;
		if (transformOptions == WICBitmapTransformRotate90 || transformOptions == WICBitmapTransformRotate270)
			determineSize(imageWidth, imageHeight, maxHeight, maxWidth, &scaledWidth, &scaledHeight);
		else
			determineSize(imageWidth, imageHeight, maxWidth, maxHeight, &scaledWidth, &scaledHeight);
		CComPtr<IWICBitmapScaler> WICBitmapScaler;
		if (!(imageWidth == maxWidth && imageHeight == maxHeight))
		{
			L_ENSURE_HR(_WICFactory->CreateBitmapScaler(&WICBitmapScaler));
			L_ENSURE_HR(WICBitmapScaler->Initialize(bitmapSource, scaledWidth, scaledHeight, WICBitmapInterpolationModeLinear));
			bitmapSource = WICBitmapScaler;
		}

		if (!(scaledWidth == maxWidth && scaledHeight == maxHeight) || transformOptions)
			buffer.resize(scaledWidth * scaledHeight * 3);

		if (buffer.empty()) {
			L_CHECK_HR(bitmapSource->CopyPixels(nullptr, scaledWidth * 3, buf_size, buf));
		}
		else {
			L_CHECK_HR(bitmapSource->CopyPixels(nullptr, scaledWidth * 3, UINT(buffer.size()), buffer.data()));
		}
		const unsigned char *ptr = buffer.data();
		std::vector<unsigned char> buffer2;
		if (transformOptions)
		{
			buffer2.resize(scaledHeight*scaledWidth * 3);
			rotate(buffer.data(), buffer2.data(), scaledWidth, scaledHeight, transformOptions, &scaledWidth, &scaledHeight, _WICFactory);
			ptr = buffer2.data();
		}

		if (!buffer.empty())
		{
			memset(buf, 0, buf_size);
			uint32_t begin_width, begin_height, end_height;

			begin_width = (maxWidth - scaledWidth) / 2;
			begin_height = (maxHeight - scaledHeight) / 2;
			end_height = begin_height + scaledHeight;
			for (uint32_t i_x = begin_height; i_x < end_height; ++i_x) {
				memcpy(buf + i_x * maxWidth * 3 + begin_width * 3, ptr, scaledWidth * 3);
				ptr += scaledWidth * 3;
			}
		}

		++_indexOfFrame;
		_WICBitmapFrameDecoder.Release();
	}

	void WindowsImageDecoder::getResolution(uint32_t* width, uint32_t* height)
	{
		if (!_WICBitmapFrameDecoder) {
			L_CHECK_HR(_WICBitmapDecoder->GetFrame(_indexOfFrame, &_WICBitmapFrameDecoder));
		}
		L_CHECK_HR(_WICBitmapFrameDecoder->GetSize(width, height));
	}

	void WindowsImageDecoder::getDPI(double* dpix, double* dpiy)
	{
		if (!_WICBitmapFrameDecoder) {
			L_CHECK_HR(_WICBitmapDecoder->GetFrame(_indexOfFrame, &_WICBitmapFrameDecoder));
		}
		L_CHECK_HR(_WICBitmapFrameDecoder->GetResolution(dpix, dpiy));
	}

	uint32_t WindowsImageDecoder::getBufferSize()
	{
		uint32_t width, height;
		if (!_WICBitmapFrameDecoder) {
			L_CHECK_HR(_WICBitmapDecoder->GetFrame(_indexOfFrame, &_WICBitmapFrameDecoder));
		}
		L_CHECK_HR(_WICBitmapFrameDecoder->GetSize(&width, &height));
		size_t buf_size = width * height * 3;
		L_CHECK_LE(buf_size, std::numeric_limits<UINT>::max());
		return (uint32_t)buf_size;
	}

	uint32_t WindowsImageDecoder::getNumberOfFrames()
	{
		return _numberOfFrames;
	}

	GUID WindowsImageDecoder::getFormat()
	{
		return MEDIASUBTYPE_RGB24;
	}

	void WindowsImageDecoder::setIndexOfFrame(uint32_t index)
	{
		L_CHECK_LT(index, _numberOfFrames);
		_indexOfFrame = index;
	}

	uint32_t WindowsImageDecoder::getDelay()
	{
		if (!_WICBitmapFrameDecoder) {
			L_CHECK_HR(_WICBitmapDecoder->GetFrame(_indexOfFrame, &_WICBitmapFrameDecoder));
		}
		CComPtr<IWICMetadataQueryReader> WICMetadataQueryReader;
		UINT delay = 0;
		HRESULT hr = _WICBitmapFrameDecoder->GetMetadataQueryReader(&WICMetadataQueryReader);
		if (FAILED(hr))
			return delay;
		PROPVARIANT propValue;
		PropVariantInit(&propValue);

		hr = WICMetadataQueryReader->GetMetadataByName(
			L"/grctlext/Delay",
			&propValue);
		if (SUCCEEDED(hr))
		{
			hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
		}
		if (SUCCEEDED(hr))
		{
			// Convert the delay retrieved in 10 ms units to a delay in 1 ms units
			hr = UIntMult(propValue.uiVal, 10, &delay);
		}
		PropVariantClear(&propValue);

		return delay;
	}

	WindowsImageDecoder::WindowsImageDecoder(const unsigned char* data, uint32_t data_size)
		: _indexOfFrame(0)
	{
		L_ENSURE_HR(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void **>(&_WICFactory)));

		L_ENSURE_HR(_WICFactory->CreateStream(&_WICStream));
		L_ENSURE_HR(_WICFactory->CreateFormatConverter(&_WICFormatConverter));
		L_ENSURE_HR(_WICStream->InitializeFromMemory((PBYTE)data, data_size));
		L_CHECK_HR(_WICFactory->CreateDecoderFromStream(_WICStream, nullptr, WICDecodeMetadataCacheOnDemand, &_WICBitmapDecoder));
		L_ENSURE_HR(_WICFactory->CreatePalette(&_WICPalette));
		HRESULT hr = _WICBitmapDecoder->CopyPalette(_WICPalette);
		if (FAILED(hr))
		{
			L_CHECK_EQ_HR(hr, WINCODEC_ERR_PALETTEUNAVAILABLE);
			_globalPaletteAvaliableBit = false;
		}
		else
			_globalPaletteAvaliableBit = true;
		L_CHECK_HR(_WICBitmapDecoder->GetFrameCount(&_numberOfFrames));
	}
}
#endif