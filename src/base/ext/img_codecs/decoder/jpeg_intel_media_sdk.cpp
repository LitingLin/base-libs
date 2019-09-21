#include <base/ext/img_codecs/decoder/jpeg.h>

#include <base/logging.h>

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

		mfxVideoParam param;
		L_CHECK_EQ(MFXVideoDECODE_DecodeHeader(_session, &bitstream, &param), MFX_ERR_NONE);
		L_CHECK_EQ(MFXVideoDECODE_Init(_session, &param), MFX_ERR_NONE);
		mfxFrameSurface1 buffer;
		memset(&buffer, 0, sizeof(mfxFrameSurface1));

		

		
		mfxFrameSurface1* output;
		L_CHECK_EQ(MFXVideoDECODE_DecodeFrameAsync(_session, &bitstream, &buffer, &output, &_syncPoint), MFX_ERR_NONE);
		L_CHECK_EQ(output, &buffer);
	}
	void IntelGraphicsJpegDecoder::decode(void* buffer)
	{
		MFXVideoCORE_SyncOperation(_session, _syncPoint, INFINITE);
		MFXVideoDECODE_Close(_session);
	}

}