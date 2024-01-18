#include <thread>
#include <chrono>
#include <atomic>
#include <functional>

class HighResolutionTimer
{
	using Clock = std::chrono::high_resolution_clock;

public:
	HighResolutionTimer(std::function<void(double)>&& callback) : callback(std::move(callback))
	{
		thread = std::thread(&HighResolutionTimer::Run, this);
	}

	~HighResolutionTimer()
	{
		Stop();
	}

	double CurrDeltaTime() const
	{
		return currDt;
	}

	void Stop()
	{
		if(isRunning)
		{
			isRunning = false;
			thread.join();
		}
	}

private:
	std::atomic<bool> isRunning = true;
	std::function<void(double)> callback;
	std::thread thread;
	std::atomic<double> currDt = 0.0;

	void Run()
	{
		auto t1 = Clock::now();
		while(isRunning)
		{
			auto t2 = Clock::now();
			double dt = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() * 1e-9);
			t1 = t2;
			currDt = dt;
			callback(dt);
		}
	}
};
