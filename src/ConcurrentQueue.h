#pragma once

#include <atomic>
#include <exception>
#include <queue>
#include <mutex>
#include <condition_variable>

class InterruptedException : public std::exception
{
public:
	virtual const char* what() const noexcept override
	{
		return "Thread has been interrupted.";
	}
};

template <typename T>
class ConcurrentQueue
{
public:
	ConcurrentQueue() : mKill(false)
	{
	}

	ConcurrentQueue(const ConcurrentQueue& o) = delete;

	~ConcurrentQueue() = default;

	ConcurrentQueue& operator=(const ConcurrentQueue& o) = delete;

	template <typename... Args>
	void emplace(Args&&... args)
	{
		std::unique_lock<std::mutex> lck(mMutex);
		mItems.emplace(std::forward<Args>(args)...);
		lck.unlock();
		mCond.notify_one();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> lck(mMutex);
		mItems.push(item);
		lck.unlock();
		mCond.notify_one();
	}

	T pop()
	{
		std::unique_lock<std::mutex> lck(mMutex);
		while (mItems.empty())
		{
			mCond.wait(lck);

			if (mKill)
			{
				throw InterruptedException();
			}
		}

		auto item = std::move(mItems.front());
		mItems.pop();

		return item;
	}

	void notifyWaiting()
	{
		mKill = true;
		mCond.notify_all();
	}

private:
	std::queue<T> mItems;
	std::mutex mMutex;
	std::condition_variable mCond;
	std::atomic_bool mKill;
};
