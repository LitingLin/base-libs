#include <base/ext/img_codecs/decoder.h>

#include <base/logging.h>

namespace Base
{
    void ImageDecoder::load(const void *buffer, size_t size, ImageFormatType formatType)
    {
        _format = formatType;
        switch (formatType)
        {
            case ImageFormatType::JPEG:
                _jpegDecoder.load(buffer, size);
                break;
            case ImageFormatType::PNG:
                _pngDecoder.load(buffer, size);
                break;
            case ImageFormatType::WEBP:
                _webpDecoder.load(buffer, size);
                break;
			default:
				NOT_IMPLEMENTED_ERROR;
        }
    }

	unsigned ImageDecoder::getHeight() {
        switch (_format)
        {
            case ImageFormatType::JPEG:
                return _jpegDecoder.getHeight();
                break;
            case ImageFormatType::PNG:
                return _pngDecoder.getHeight();
                break;
            case ImageFormatType::WEBP:
                return _webpDecoder.getHeight();
                break;
			default:
				NOT_IMPLEMENTED_ERROR;
				return 0;
        }
    }

	unsigned ImageDecoder::getWidth() {
        switch (_format)
        {
            case ImageFormatType::JPEG:
                return _jpegDecoder.getWidth();
                break;
            case ImageFormatType::PNG:
                return _pngDecoder.getWidth();
                break;
            case ImageFormatType::WEBP:
                return _webpDecoder.getWidth();
                break;
			default:
				NOT_IMPLEMENTED_ERROR;
				return 0;
        }
    }

	uint64_t ImageDecoder::getDecompressedSize() {
        switch (_format)
        {
            case ImageFormatType::JPEG:
                return _jpegDecoder.getDecompressedSize();
                break;
            case ImageFormatType::PNG:
                return _pngDecoder.getDecompressedSize();
                break;
            case ImageFormatType::WEBP:
                return _webpDecoder.getDecompressedSize();
                break;
			default:
				NOT_IMPLEMENTED_ERROR;
				return 0;
        }
    }

    void ImageDecoder::decode(void *output) {
        switch (_format)
        {
            case ImageFormatType::JPEG:
                _jpegDecoder.decode(output);
                break;
            case ImageFormatType::PNG:
                _pngDecoder.decode(output);
                break;
            case ImageFormatType::WEBP:
                _webpDecoder.decode(output);
                break;
			default:
				NOT_IMPLEMENTED_ERROR;
        }
    }
}