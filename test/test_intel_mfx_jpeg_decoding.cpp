#include "pch.h"

#include <base/file.h>
#include <base/memory_alignment.h>
#include <base/ext/img_codecs/decoder/jpeg.h>

TEST(INTEL_MFX_DECODING, JPEG)
{
#ifdef _WIN32
	Base::File file(L"Lenna.jpg");
#else
	Base::File file("Lenna.jpg");
#endif
	Base::AlignedMemorySpace jpegFile(file.getSize());
	EXPECT_EQ(file.read(jpegFile.get(), file.getSize()), file.getSize());
	Base::IntelGraphicsJpegDecoder decoder;
	decoder.load(jpegFile.get(), file.getSize());
	std::vector<uint8_t> buffer(decoder.getDecompressedSize());
	decoder.decode(buffer.data());
}