#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <base/porting.h>
#include <base/memory_mapped_io.h>
#include <base/image_decoder.h>
#include <base/utils.h>

#ifdef _WIN32
int __stdcall wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nShowCmd
)
#else
int main(int argc, char* argv[])
#endif
{
	try
	{
#ifdef _WIN32
		std::wstring fileName = lpCmdLine;
		Base::File file(lpCmdLine);
		Base::MemoryMappedIO mmap(&file);
#else
		if (argc != 2)
			return -1;
		std::string fileName = argv[1];
		Base::File file(argv[1]);
		Base::MemoryMappedIO mmap(&file);
#endif
				
		cv::_InputArray rawData((char*)mmap.get(), file.getSize());
		cv::Mat image = cv::imdecode(rawData, cv::IMREAD_COLOR);
		cv::Mat decoderImage;
		if (Base::endsWith(fileName, _T(".jpg")))
		{
			Base::JPEGDecoder decoder;
			decoder.load(mmap.get(), file.getSize());
			decoderImage = cv::Mat(decoder.getHeight(), decoder.getWidth(), CV_8UC3);
			decoder.decode(decoderImage.data);
		}
		else if(Base::endsWith(fileName, _T(".png")))
		{
			Base::PNGDecoder decoder;
			decoder.load(mmap.get(), file.getSize());
			decoderImage = cv::Mat(decoder.getHeight(), decoder.getWidth(), CV_8UC3);
			decoder.decode(decoderImage.data);
		}
		else
			return -2;
		cv::cvtColor(decoderImage, decoderImage, cv::COLOR_RGB2BGR);
		cv::imshow("opencv", image);
		cv::imshow("decoder", decoderImage);
		cv::waitKey();
	}
	catch (...)
	{
		return -1;
	}
	return 0;
}
