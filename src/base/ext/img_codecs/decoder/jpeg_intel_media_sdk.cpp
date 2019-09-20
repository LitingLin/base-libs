#include <base/ext/img_codecs/decoder/jpeg.h>

#include <base/logging.h>

namespace Base
{
	typedef struct {
		mfxU16 width, height;
		mfxU8* base;
	} mid_struct;
	mfxStatus fa_alloc(mfxHDL pthis, mfxFrameAllocRequest* request, mfxFrameAllocResponse* response)
	{
		if (!(request->Type & MFX_MEMTYPE_SYSTEM_MEMORY))
			return MFX_ERR_UNSUPPORTED;
		if (request->Info.FourCC != MFX_FOURCC_NV12)
			return MFX_ERR_UNSUPPORTED;
		response->NumFrameActual = request->NumFrameMin;
		for (int i = 0; i < request->NumFrameMin; i++) {
			mid_struct* mmid = (mid_struct*)malloc(sizeof(mid_struct));
			mmid->width = ALIGN32(request->Info.Width);
			mmid->height = ALIGN32(request->Info.Height);
			mmid->base = (mfxU8*)malloc(mmid->width * mmid->height * 3 / 2);
			response->mids[i] = mmid;
		}
		return MFX_ERR_NONE;
	}
	mfxStatus fa_lock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr) {
		mid_struct* mmid = (mid_struct*)mid;
		ptr->pitch = mmid->width;
		ptr->Y = mmid->base;
		ptr->U = ptr->Y + mmid->width * mmid->height;
		ptr->V = ptr->U + 1;
		return MFX_ERR_NONE;
	}
	mfxStatus fa_unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr) {
		if (ptr) ptr->Y = ptr->U = ptr->V = ptr->A = 0;
		return MFX_ERR_NONE;
	}
	mfxStatus fa_gethdl(mfxHDL pthis, mfxMemId mid, mfxHDL* handle) {
		return MFX_ERR_UNSUPPORTED;
	}
	mfxStatus fa_free(mfxHDL pthis, mfxFrameAllocResponse* response) {
		for (int i = 0; i < response->NumFrameActual; i++) {
			mid_struct* mmid = (mid_struct*)response->mids[i];
			free(mmid->base); free(mid);
		}
		return MFX_ERR_NONE;
	}
	
	void IntelGraphicsJpegDecoder::decode()
	{
		mfxSession _session;
		mfxVersion _version;
		_version.Major = 1;
		_version.Minor = 0;
		L_CHECK_EQ(MFXInit(MFX_IMPL_HARDWARE, &_version, &_session), ;


		MFXVideoDECODE_DecodeHeader();

		mfxVideoParam param;
		memset(&param, 0, sizeof(param));
		param.AsyncDepth = 1;
		param.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
		param.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
		param.

		MFXVideoDECODE_Init();

			MFXClose(_session);
	}

}