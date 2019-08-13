#pragma once

namespace Base
{
	class WebPEncoder
	{
	public:
		class Container
		{
		public:
			Container(void* ptr, size_t size);
			Container(const Container&) = delete;
			Container(Container&&) = delete;
			~Container();
			void* get();
			size_t size();
		private:
			void* _ptr;
			size_t _size;
		};
		Container encode(const void* rgb, int width, int height);
	};
}