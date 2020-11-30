#include <base/ext/img_codecs/decoder.h>

#include <base/logging.h>

namespace Base
{
    void ImageDecoder::load(const void *buffer, size_t size, ImageFormatType formatType)
    {
        _format = formatType;
        switch (formatType)
        {
#if (defined HAVE_LIB_JPEG) || (defined HAVE_LIB_JPEG_TURBO)
            case ImageFormatType::JPEG:
                _jpegDecoder.load(buffer, size);
                break;
#endif
#if (defined HAVE_LIB_PNG)
            case ImageFormatType::PNG:
                _pngDecoder.load(buffer, size);
                break;
#endif
#if (defined HAVE_LIB_WEBP)
            case ImageFormatType::WEBP:
                _webpDecoder.load(buffer, size);
                break;
#endif
			default:
				L_NOT_IMPLEMENTED_ERROR;
        }
    }

	unsigned ImageDecoder::getHeight() const {
        switch (_format)
        {
#if (defined HAVE_LIB_JPEG) || (defined HAVE_LIB_JPEG_TURBO)
            case ImageFormatType::JPEG:
                return _jpegDecoder.getHeight();
                break;
#endif
#if (defined HAVE_LIB_PNG)
            case ImageFormatType::PNG:
                return _pngDecoder.getHeight();
                break;
#endif
#if (defined HAVE_LIB_WEBP)
            case ImageFormatType::WEBP:
                return _webpDecoder.getHeight();
                break;
#endif
			default:
				L_NOT_IMPLEMENTED_ERROR;
				return 0;
        }
    }

	unsigned ImageDecoder::getWidth() const {
        switch (_format)
        {
#if (defined HAVE_LIB_JPEG) || (defined HAVE_LIB_JPEG_TURBO)
            case ImageFormatType::JPEG:
                return _jpegDecoder.getWidth();
                break;
#endif
#if (defined HAVE_LIB_PNG)
            case ImageFormatType::PNG:
                return _pngDecoder.getWidth();
                break;
#endif
#if (defined HAVE_LIB_WEBP)
            case ImageFormatType::WEBP:
                return _webpDecoder.getWidth();
                break;
#endif
			default:
				L_NOT_IMPLEMENTED_ERROR;
				return 0;
        }
    }

	uint64_t ImageDecoder::getDecompressedSize() const {
        switch (_format)
        {
#if (defined HAVE_LIB_JPEG) || (defined HAVE_LIB_JPEG_TURBO)
            case ImageFormatType::JPEG:
                return _jpegDecoder.getDecompressedSize();
                break;
#endif
#if (defined HAVE_LIB_PNG)
            case ImageFormatType::PNG:
                return _pngDecoder.getDecompressedSize();
                break;
#endif
#if (defined HAVE_LIB_WEBP)
            case ImageFormatType::WEBP:
                return _webpDecoder.getDecompressedSize();
                break;
#endif
			default:
				L_NOT_IMPLEMENTED_ERROR;
				return 0;
        }
    }

    void ImageDecoder::decode(void *output) {
        switch (_format)
        {
#if (defined HAVE_LIB_JPEG) || (defined HAVE_LIB_JPEG_TURBO)
            case ImageFormatType::JPEG:
                _jpegDecoder.decode(output);
                break;
#endif
#if (defined HAVE_LIB_PNG)
            case ImageFormatType::PNG:
                _pngDecoder.decode(output);
                break;
#endif
#if (defined HAVE_LIB_WEBP)
            case ImageFormatType::WEBP:
                _webpDecoder.decode(output);
                break;
#endif
			default:
				L_NOT_IMPLEMENTED_ERROR;
        }
    }
}