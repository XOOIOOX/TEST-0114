#include <iostream>
#include <vector>
#include <shared_mutex>

#define numWriters		100
#define numReaders		10
#define writeDelayMs	400
#define readDelayMs		100
#define repeatWhole		10

using VectorInt = std::vector<int>;
using Thread = std::thread;
using VectorThreads = std::vector<Thread>;
using SharedMutex = std::shared_mutex;
using LockGuard = std::lock_guard<SharedMutex>;

VectorInt vec;
SharedMutex mutex;
VectorThreads vectorThreads;

void threadWrite()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(writeDelayMs));

	{
		LockGuard locker(mutex);
		vec.push_back(static_cast<int>(vec.size() + 1));
		std::cout << "Added value: " << vec.back() << std::endl;
	}
};

void threadRead()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(readDelayMs));

	{
		LockGuard locker(mutex);
		if (!vec.empty())
		{
			std::cout << "Vector contains: ";
			for (const auto& i : vec)
			{
				std::cout << i << " ";
			}

			std::cout << std::endl;
		}
		else
		{
			std::cout << "Vector is empty! " << std::endl;
		}
	}
}

int main()
{
	for (size_t i = 0; i < repeatWhole; i++)
	{
		std::cout << "Iteration:" << i << std::endl;

		for (int j = 0; j < numReaders; j++)
		{
			vectorThreads.emplace_back(Thread(&threadRead));
		}

		for (int j = 0; j < numWriters; j++)
		{
			vectorThreads.emplace_back(Thread(&threadWrite));
		}

		for (auto& j : vectorThreads) { j.join(); }
		vectorThreads.clear();
	}

	return 0;
}