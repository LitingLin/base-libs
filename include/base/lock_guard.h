#pragma once
#include <shared_mutex>

namespace Base
{
	// Use std::shared_lock instead
	class __declspec(deprecated) SharedLockGuard_LockShared
	{
	public:
		SharedLockGuard_LockShared(std::shared_mutex &lock);
		~SharedLockGuard_LockShared();
		void unlock();
	private:
		std::shared_mutex *_lock;
	};

	// Use std::unique_lock instead
	class __declspec(deprecated) SharedLockGuard
	{
	public:
		SharedLockGuard(std::shared_mutex &lock);
		~SharedLockGuard();
		void unlock();
	private:
		std::shared_mutex *_lock;
	};
}