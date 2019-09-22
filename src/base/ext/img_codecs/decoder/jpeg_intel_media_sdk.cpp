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
		L_CHECK_EQ(MFXInit(MFX_IMPL_HARDWARE, &version, &_session), MFX_ERR_NONE);
	}

	IntelGraphicsJpegDecoder::~IntelGraphicsJpegDecoder()
	{
		L_LOG_IF_NOT_EQ(MFXClose(_session), MFX_ERR_NONE);
	}

	void IntelGraphicsJpegDecoder::load(const void* data, size_t size)
	{
		mfxBitstream bitstream;
		memset(&bitstream, 0, sizeof(bitstream));
		bitstream.DecodeTimeStamp = MFX_TIMESTAMP_UNKNOWN;
		bitstream.TimeStamp = MFX_TIMESTAMP_UNKNOWN;
		bitstream.Data = (mfxU8*)data;
		bitstream.DataLength = size;
		bitstream.MaxLength = size;
		bitstream.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;

		memset(&_param, 0, sizeof(_param));
		L_CHECK_EQ(MFXVideoDECODE_DecodeHeader(_session, &bitstream, &_param), MFX_ERR_NONE);
		L_CHECK_EQ(MFXVideoDECODE_Init(_session, &_param), MFX_ERR_NONE);
		mfxFrameSurface1 buffer;
		memset(&buffer, 0, sizeof(mfxFrameSurface1));

		buffer.Info = _param.mfx.FrameInfo;
		buffer.Data.TimeStamp = MFX_TIMESTAMP_UNKNOWN;
		buffer.Data.MemType = MFX_MEMTYPE_SYSTEM_MEMORY;

		auto height = _param.mfx.FrameInfo.Height;
		auto width = _param.mfx.FrameInfo.Width;
		auto format = _param.mfx.FrameInfo.FourCC;

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
		
		mfxFrameSurface1* output;
		L_CHECK_EQ(MFXVideoDECODE_DecodeFrameAsync(_session, &bitstream, &buffer, &output, &_syncPoint), MFX_ERR_NONE);
		L_CHECK_EQ(output, &buffer);
	}
	void IntelGraphicsJpegDecoder::decode(void* buffer)
	{
		MFXVideoCORE_SyncOperation(_session, _syncPoint, INFINITE);
		MFXVideoDECODE_Close(_session);

		sws_getContext()
	}

}