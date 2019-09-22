#include <base/ext/img_codecs/decoder/jpeg.h>

#include <base/logging.h>

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace Base
{
	IntelGraphicsJpegDecoder::IntelGraphicsJpegDecoder()
		: _currentHandle(0)
	{
		mfxVersion version;
		version.Major = 1;
		version.Minor = 0;
		L_CHECK_EQ(MFXInit(MFX_IMPL_HARDWARE_ANY, &version, &_session), MFX_ERR_NONE);
	}

	IntelGraphicsJpegDecoder::~IntelGraphicsJpegDecoder()
	{
		L_LOG_IF_NOT_EQ(MFXClose(_session), MFX_ERR_NONE);
	}

	void IntelGraphicsJpegDecoder::load(const void* data, size_t size)
	{
		L_CHECK_LE(size, std::numeric_limits<uint32_t>::max());
		mfxBitstream bitstream;
		memset(&bitstream, 0, sizeof(bitstream));
		bitstream.DecodeTimeStamp = MFX_TIMESTAMP_UNKNOWN;
		bitstream.TimeStamp = (mfxU64)MFX_TIMESTAMP_UNKNOWN;
		bitstream.Data = (mfxU8*)data;
		bitstream.DataLength = (mfxU32)size;
		bitstream.MaxLength = (mfxU32)size;
		bitstream.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;

		memset(&_param, 0, sizeof(_param));
		L_CHECK_EQ(MFXVideoDECODE_DecodeHeader(_session, &bitstream, &_param), MFX_ERR_NONE);
		L_CHECK_EQ(MFXVideoDECODE_Init(_session, &_param), MFX_ERR_NONE);
		mfxFrameSurface1 buffer;
		memset(&buffer, 0, sizeof(mfxFrameSurface1));

		buffer.Info = _param.mfx.FrameInfo;
		buffer.Data.TimeStamp = (mfxU64)MFX_TIMESTAMP_UNKNOWN;
		buffer.Data.MemType = MFX_MEMTYPE_SYSTEM_MEMORY;

		auto height = _param.mfx.FrameInfo.Height;
		auto width = _param.mfx.FrameInfo.Width;
		auto format = _param.mfx.FrameInfo.FourCC;

		L_CHECK_GT(_param.mfx.FrameInfo.CropW, 0);
		L_CHECK_GT(_param.mfx.FrameInfo.CropH, 0);

		if (format == MFX_FOURCC_NV12)
		{
			L_CHECK_EQ(_param.mfx.FrameInfo.ChromaFormat, MFX_CHROMAFORMAT_YUV420);
			auto bufferSize = height * width * 3U / 2U;
			
			if (_buffer.size() < bufferSize)
				_buffer.resize(bufferSize);

			uint8_t* bufferPointer = (uint8_t*)_buffer.get();
			buffer.Data.Y = bufferPointer;
			buffer.Data.U = buffer.Data.Y + (uint32_t)width * (uint32_t)height;
			buffer.Data.V = buffer.Data.U + 1;
			
			buffer.Data.PitchHigh = 0;
			buffer.Data.PitchLow = width;
		}
		else if (format == MFX_FOURCC_YUY2)
		{
			L_CHECK_EQ(_param.mfx.FrameInfo.ChromaFormat, MFX_CHROMAFORMAT_YUV422);
			auto bufferSize = height * width * 2U;

			if (_buffer.size() < bufferSize)
				_buffer.resize(bufferSize);

			uint8_t* bufferPointer = (uint8_t*)_buffer.get();
			
			buffer.Data.Y = bufferPointer;
			buffer.Data.U = bufferPointer + 1;
			buffer.Data.V = bufferPointer + 3;
			
			uint32_t pitch = (uint32_t)width * 2;

			buffer.Data.PitchHigh = uint16_t(pitch >> 16);
			buffer.Data.PitchLow = uint16_t(pitch);
		}
		else if (format == MFX_FOURCC_RGB4)
		{
			L_CHECK_EQ(_param.mfx.FrameInfo.ChromaFormat, MFX_CHROMAFORMAT_YUV444);
			auto bufferSize = height * width * 4U;

			if (_buffer.size() < bufferSize)
				_buffer.resize(bufferSize);

			uint8_t* bufferPointer = (uint8_t*)_buffer.get();
			buffer.Data.R = bufferPointer;
			buffer.Data.G = bufferPointer + 1;
			buffer.Data.B = bufferPointer + 2;
			buffer.Data.A = bufferPointer + 3;

			uint32_t pitch = (uint32_t)width * 4;
			
			buffer.Data.PitchHigh = uint16_t(pitch >> 16);
			buffer.Data.PitchLow = uint16_t(pitch);
		}
		else
		{
			L_UNREACHABLE_ERROR;
		}
		
		mfxFrameSurface1* output;
		L_CHECK_EQ(MFXVideoDECODE_DecodeFrameAsync(_session, &bitstream, &buffer, &output, &_syncPoint), MFX_ERR_NONE);
		L_CHECK_EQ(output, &buffer);
	}

	unsigned IntelGraphicsJpegDecoder::getWidth()
	{
		return _param.mfx.FrameInfo.CropW;
	}

	unsigned IntelGraphicsJpegDecoder::getHeight()
	{
		return _param.mfx.FrameInfo.CropH;
	}

	uint64_t IntelGraphicsJpegDecoder::getDecompressedSize()
	{
		return uint64_t(_param.mfx.FrameInfo.CropW) * uint64_t(_param.mfx.FrameInfo.CropH) * 3;
	}

	void IntelGraphicsJpegDecoder::decode(void* buffer)
	{
		L_CHECK_EQ(MFXVideoCORE_SyncOperation(_session, _syncPoint, INFINITE), MFX_ERR_NONE);
		L_CHECK_EQ(MFXVideoDECODE_Close(_session), MFX_ERR_NONE);

		AVPixelFormat sourceFormat;
		switch (_param.mfx.FrameInfo.FourCC)
		{
		case MFX_FOURCC_NV12:
			sourceFormat = AV_PIX_FMT_NV12;
			break;
		case MFX_FOURCC_YUY2:
			sourceFormat = AV_PIX_FMT_YUYV422;
			break;
		case MFX_FOURCC_RGB4:
			sourceFormat = AV_PIX_FMT_RGBA;
			break;
		default:
			L_UNREACHABLE_ERROR;
		}

		_formatTransformer.transform(_param.mfx.FrameInfo.Width, _param.mfx.FrameInfo.Height, sourceFormat,
			_param.mfx.FrameInfo.CropX, _param.mfx.FrameInfo.CropY, _param.mfx.FrameInfo.CropW, _param.mfx.FrameInfo.CropH,
			_param.mfx.FrameInfo.CropW, _param.mfx.FrameInfo.CropH, AV_PIX_FMT_RGB24, 
			0, 0, _param.mfx.FrameInfo.CropW, _param.mfx.FrameInfo.CropH,
			false, (uint8_t*)_buffer.get(), (uint8_t*)buffer);
	}
}